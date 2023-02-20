//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "terrainshader.h"
#include "ShaderLightmapper.h"
#include <iostream>
#include <random>
#include <vector>
#include "Text.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

#define MAX_STARS 10
#define MAX_BUILDINGS 10
#define MAX_TRAVEL_DIST 4

Application::Application(GLFWwindow* pWin, float wWidth, float wHeight) :
	pWindow(pWin), Cam(pWin),
	pModel(NULL), ShadowGenerator(2048, 2048),
	windowWidth(wWidth),
	windowHeight(wHeight),
	text(Cam, windowWidth, windowHeight),
	textDied(Cam, windowWidth, windowHeight),
	points(0),
	startTime(0),
	deductionTime(0),
	option(false),
	pointDeduction(false)
{
	createSantaWorld();
	this->text.loadFont(ASSET_DIRECTORY "fonts/OpenSans-Medium.ttf", 24);
	this->textDied.loadFont(ASSET_DIRECTORY "fonts/OpenSans-SemiBoldItalic.ttf", 28);
}

/// <summary>
/// Laedt die Welt mit den verschiedenen Gebaueden, dem Rentierschlitten und Lichtern.
/// </summary>
void Application::createSantaWorld() {
	// Umgebung als Cube: Skybox
	this->pSky = new Model(ASSET_DIRECTORY "skybox.obj", false);
	this->pSky->shader(new PhongShader(), true);
	this->pSky->shadowCaster(false);
	this->pSky->transform(pSky->transform() * Matrix().scale(1.5));
	this->pSky->transformBoundingBox(this->pSky->transform());
	Models.push_back(pSky);

	// Boden
	this->pGround = new Model(ASSET_DIRECTORY "buildings/Ground.obj", false);
	this->pGround->shader(new PhongShader(), true);
	this->pGround->shadowCaster(false);
	this->pGround->transform(pGround->transform() * Matrix().scale(3));
	this->pGround->transformBoundingBox(this->pGround->transform());
	Models.push_back(pGround);

	// Erstellen der Map
	this->pCity = new City();
	std::vector<const char*> buildings;
	buildings.push_back(ASSET_DIRECTORY "buildings/Brick_Building/Brick_Building.obj");
	buildings.push_back(ASSET_DIRECTORY "buildings/FairyTower/FairyTower.obj");
	buildings.push_back(ASSET_DIRECTORY "buildings/Residential_House/Residential_House.obj");
	this->pCity->loadModels(buildings, 5, 5, 40);
	Models.push_back(pCity);

	// Erstellen des Schlittens
	this->pSantaSleigh = new SantaSleigh();
	this->pSantaSleigh->shader(new PhongShader(), true);
	this->pSantaSleigh->loadModels(
		ASSET_DIRECTORY "deer.obj",
		ASSET_DIRECTORY "santasleigh.obj");
	this->pSantaSleigh->transform(
		this->pSantaSleigh->transform()
		* Matrix().translation(this->pSantaSleigh->getStartPos()));
	Models.push_back(pSantaSleigh);
	this->pGiftTravel = MAX_TRAVEL_DIST;

	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);

	this->startTime = glfwGetTime();
}

/// <summary>
/// Aktiviert Tiefenvergleich, das nicht Rendern von Rueckseiten, Farbmischung und korrekte Transparenzmischung.
/// </summary>
void Application::start() {
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/// <summary>
/// Aktuallisiert die Szene, wird pro Frame aufgerufen.
/// </summary>
/// <param name="dtime">Zeit zwischen zwei Frames</param>
void Application::update(float dtime) {
	float upDown = 0;		// xRot
	float leftRight = 0;	// yRot
	float shift = 0;		// zRot
	bool drive = false;
	this->keyboardInput(upDown, leftRight, shift, drive);

	this->pSantaSleigh->steer(upDown, leftRight, shift, drive);
	this->pSantaSleigh->update(dtime);

	Matrix mCam, mDistance;
	mDistance.translation(Vector(0, 4, -15));
	mCam = this->pSantaSleigh->transform() * mDistance;

	// Kollisionserkennung Schlitten mit allen Gebäuden
	for (Building* model : this->pCity->getModels()) {
		model->update(dtime);
		if (this->checkCollision(this->pSantaSleigh->sleigh, model->building)) {
			this->pSantaSleigh->reset();
			this->pointDeduction = true;
			this->deductionTime = glfwGetTime();
		}
	}

	// Kollisionserkennung Schlitten mit Umgebung
	if (this->checkCollision(this->pSantaSleigh->sleigh, this->pGround) 
		|| !this->checkCollision(this->pSantaSleigh->sleigh, this->pSky)) {
		this->pSantaSleigh->reset();
		this->pointDeduction = true;
		this->deductionTime = glfwGetTime();
	}

	// Schießen
	if (glfwGetKey(this->pWindow, GLFW_KEY_R)) {
		if (!isGifting) {
			pGift = new Model();
			pGift->shader(new PhongShader(), false);
			pGift->load(ASSET_DIRECTORY "Gold_Star.obj");
			pGift->transform(pSantaSleigh->sleigh->transform() * Matrix().scale(0.08));
			pGift->transformBoundingBox(pGift->transform());
			Models.push_back(pGift);
			this->isGifting = true;
		}
	}

	if (this->isGifting) {
		this->pGiftTravel -= 1 * dtime;
		if (this->pGiftTravel <= 1) {
			Models.remove(this->pGift);
			this->pGiftTravel = MAX_TRAVEL_DIST;
			this->isGifting = false;
			return;
		}

		for (Building* building : this->pCity->getTargets()) {
			if (checkCollision(this->pGift, building->building)) {
				this->points++;
				this->isGifting = false;
				this->Models.remove(this->pGift);
				building->removeTarget();
				this->pCity->removeTarget(building);
				this->pGiftTravel = MAX_TRAVEL_DIST;
				return;
			}
		}

		Matrix m;
		m.translation(0, 0, 400 * dtime);
		this->pGift->transform(this->pGift->transform() * m);
		this->pGift->transformBoundingBox(this->pGift->transform());
	}

	Cam.setPosition(mCam.translation());
	Cam.setTarget(this->pSantaSleigh->getPosition());
	Cam.setUp(mCam.up());

    Cam.update();
}

/// <summary>
/// Ruft Draw Methoden der einzelnen Objekte der ModelLauf, ruft die Methode zum Rendern der Texte auf
/// und prueft auf geworfene OpenGL-Fehler innerhalb des Programms.
/// </summary>
void Application::draw() {
	ShadowGenerator.generate(Models);
	
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ShaderLightMapper::instance().activate();
    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it ) {
        (*it)->draw(Cam);
    }
	ShaderLightMapper::instance().deactivate();
	
	// show texts
	this->showTexts();
	
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}

/// <summary>
/// Loescht alle Objekte der ModelListe.
/// </summary>
void Application::end() {
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}

/// <summary>
/// Die Methode erkennt bestimmte Tastatureingaben und setzt entsprechende Werte
/// </summary>
/// <param name="xRot">Wert fuer Pitch-Bewegung</param>
/// <param name="yRot">Wert fuer Yaw-Bewegung</param>
/// <param name="zRot">Wert fuer Roll-Bewegung</param>
/// <param name="drive">Ob Schlitten faehrt</param>
void Application::keyboardInput(float& xRot, float& yRot, float& zRot, bool& drive) {
	if (glfwGetKey(this->pWindow, GLFW_KEY_W)) { xRot = 1.2; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_S)) { xRot = -1.2; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_A)) { zRot = -1.5; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_D)) { zRot = 1.5; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_Q)) { yRot = 1.2; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_E)) { yRot = -1.2; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_SPACE)) {
		drive = true;
	}

	if(glfwGetKey(this->pWindow, GLFW_KEY_O)) { this->option = true; }
}

/// <summary>
/// Sammlung der Textaufrufe. Diese geschehen teilweise nur unter bestimmten Bedingungen.
/// </summary>
void Application::showTexts() {
	double currentTime = glfwGetTime();
	double durationOption = currentTime - this->startTime;
	if (this->option) {
		durationOption = 0;
		this->startTime = glfwGetTime();
		this->option = false;
	}
	if (durationOption <= 4.5) { this->showKeyBindings(); }

	if (this->pointDeduction) {
		this->showPointDeduction();
		double durationDeduction = currentTime - this->deductionTime;
		if (durationDeduction >= 3.0) { this->pointDeduction = false; }
	}

	this->showPoints();
}

/// <summary>
/// Erzeugt einen weissen Text am oberen Linken Bildschirmrand mit der aktuellen Punktezahl.
/// </summary>
void Application::showPoints() {
	std::string text = "Punkte: ";
	text += std::to_string(this->points);

	float padding = 20.0;
	float left = -1 * (this->windowWidth / 2 - padding);
	float top = (this->windowHeight / 2) - (2 * padding);
	Color color = Color(1.0, 1.0, 1.0);

	this->text.renderText(Cam, text, left, top, 1.0f, color);
};

/// <summary>
/// Erzeugt schwarzen Text am unteren linken Bildschirmrand, der die Steuerung angibt.
/// </summary>
void Application::showKeyBindings() {
	float left = -1 * (this->windowWidth / 2 - 20);
	float bottom = -1 * (this->windowHeight / 2);
	Color color = Color(0.0, 0.0, 0.0);

	this->text.renderText(Cam, "Steuerung anzeigen: O", left, bottom + 170, 1.0f, color);
	this->text.renderText(Cam, "Pitch: W/S", left, bottom + 140, 1.0f, color);
	this->text.renderText(Cam, "Roll: A/D", left, bottom + 110, 1.0f, color);
	this->text.renderText(Cam, "Yaw: Q/E", left, bottom + 80, 1.0f, color);
	this->text.renderText(Cam, "Beschleunigen: Space", left, bottom + 50, 1.0f, color);
	this->text.renderText(Cam, "Stern schiessen: R", left, bottom + 20, 1.0f, color);
}

/// <summary>
/// Erzeugt einen roten italic Text am oberen rechten Bildschirmrand, mit Todes-Information.
/// </summary>
void Application::showPointDeduction() {
	std::string text = "Du bist gestorben";
	float right = this->windowWidth / 2 - 250;
	float top = (this->windowHeight / 2) - 40;
	Color color = Color(1.0, 0.1, 0.3);

	this->textDied.renderText(Cam, text, right, top, 1.0f, color);
}

/// <summary>
/// Debugmethode zum Zeichnen der Boundingbox.
/// </summary>
/// <param name="box">Boundingbox</param>
void Application::drawBoundingBox(AABB box) {
	float width = box.Max.X - box.Min.X;
	float height = box.Max.Y - box.Min.Y;
	float depth = box.Max.Z - box.Min.Z;

	this->pModel = new LineBoxModel(width, height, depth);
	ConstantShader* pConstantShader = new ConstantShader();
	pConstantShader->color(Color(0, 1, 0));
	this->pModel->shader(pConstantShader, true);
	this->pModel->transform(Matrix().translation(0, height / 2, 0));
	this->Models.push_back(pModel);
}

/// <summary>
/// Kollisionserkennung zwischen zwei Modellen, anhand der Boundingboxen.
/// </summary>
/// <param name="model_a">Model eins</param>
/// <param name="model_b">Model zwei</param>
/// <returns></returns>
bool Application::checkCollision(BaseModel* model_a, BaseModel* model_b) {
	AABB bbox_a = model_a->boundingBox();
	AABB bbox_b = model_b->boundingBox();
	bbox_a = bbox_a.transform(model_a->transform());
	bool collision = false;

	Vector minB = bbox_b.Min;
	Vector maxB = bbox_b.Max;
	Vector minS = bbox_a.Min;
	Vector maxS = bbox_a.Max;

	if ( (bbox_a.Max.X >= bbox_b.Min.X && bbox_a.Min.X <= bbox_b.Max.X)
		&& (bbox_a.Max.Y >= bbox_b.Min.Y && bbox_a.Min.Y <= bbox_b.Max.Y)
		&& (bbox_a.Max.Z >= bbox_b.Min.Z && bbox_a.Min.Z <= bbox_b.Max.Z)) {
		collision = true;
	}
	return collision;
}
