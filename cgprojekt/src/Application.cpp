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
#include "ShaderLightmapper.h"
#include <iostream>
#include <random>


#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

#define MAX_STARS 10
#define MAX_BUILDINGS 10
#define MAX_TRAVEL_DIST 4

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048) {
	createScene();
}

void Application::createScene() {
	// Umgebung als Cube: Skybox
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	pModel->transform(pModel->transform() * Matrix().scale(5));
	Models.push_back(pModel);

	/* for (int i = 0; i < MAX_STARS; i++) {
		pModel = new Model(ASSET_DIRECTORY "Gold_Star.obj", false);
		pModel->shader(new PhongShader(), true);
		pModel->transform(pModel->transform() * this->randomTranslation());
		pModel->transform(pModel->transform() * Matrix().scale(0.1));
		Stars.push_back(pModel);
		Models.push_back(pModel);
	} */

	this->pCity = new City();
	this->pCity->loadModels(ASSET_DIRECTORY "3d-model.obj", 4, 4, 10);
	Models.push_back(pCity);

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

	/* AABB sleighBox = this->pSantaSleigh->sleigh->boundingBox();
	float height = pSantaSleigh->sleigh->boundingBox().Max.Y - pSantaSleigh->sleigh->boundingBox().Min.Y;
	this->drawBoundingBox(this->pSantaSleigh->sleigh->boundingBox().transform(this->pSantaSleigh->transform() * Matrix().translation(0, height / 2, 0))); */
	 
	// Globale Lichtquelle
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
}

void Application::start() {
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float dtime) {
	float upDown = 0;		// xRot
	float leftRight = 0;	// yRot
	float shift = 0;		// zRot
	bool drive = false;
	this->keyboardInput(upDown, leftRight, shift, drive);

	this->pSantaSleigh->steer(upDown, leftRight, shift, drive);
	this->pSantaSleigh->update(dtime);
	// ggf. fuer weichere Camerabewegung nutzbar ------------------------------
	// Laenge der Objektbewegung berechnen und auf Cam-Position anwenden
	float smoothness = 1;
	Vector currentObjPos, lastObjPos, travelDist, camPos;
	currentObjPos = this->pSantaSleigh->getPosition();
	lastObjPos = this->pSantaSleigh->getLastPosition();
	travelDist = currentObjPos - lastObjPos;
	camPos = Cam.position() + (travelDist * smoothness);
	// Letzte Position des Objekts setzen
	this->pSantaSleigh->setLastPosition(currentObjPos);
	// ------------------------------------------------------------------------

	Matrix mCam, mDistance;
	mDistance.translation(Vector(0, 4, -15));
	mCam = this->pSantaSleigh->transform() * mDistance;

	for (Building* model : this->pCity->getModels()) {
		if (this->checkCollision(this->pSantaSleigh->sleigh, model->building)) {
			this->pSantaSleigh->reset();
		}
	}

	if (glfwGetKey(this->pWindow, GLFW_KEY_R)) {
		/* double xPos, yPos;
		glfwGetCursorPos(this->pWindow, &xPos, &yPos);
		int wWidth, wHeight;
		glfwGetWindowSize(this->pWindow, &wWidth, &wHeight);

		// Normalisierung der Mauskoordinaten
		float xMouse = xPos / wWidth * 2 - 1;
		float yMouse = -(yPos / wHeight * 2 - 1);
		Vector r;
		Vector d = calc3DRay(xMouse, yMouse, r);
		float s = -r.Y / d.Y;
		Vector c = r + (d * s); */
		if (!isGifting) {
			pGift = new Model();
			pGift->shader(new PhongShader(), false);
			pGift->load(ASSET_DIRECTORY "Gold_Star.obj");
			Matrix m;
			m.scale(0.15);
			pGift->transform(pSantaSleigh->sleigh->transform() * m);
			pGift->transformBoundingBox(pSantaSleigh->sleigh->transform());
			Models.push_back(pGift);
			this->isGifting = true;
		}
		//std::cout << "Click: " << "X: " << xPos << " Y: " << yPos << std::endl; 
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
			if (checkGiftCollision(this->pGift, building->building)) {
				std::cout << "DELIVERED!" << std::endl;
				this->Models.remove(this->pGift);
				building->removeTarget();
			}
		}

		Matrix m;
		//m.rotationZ(45 * dtime);
		m.translation(0, 0, 200 * dtime);
		this->pGift->transform(this->pGift->transform() * m);
		this->pGift->transformBoundingBox(this->pGift->transform());
	}

	if (glfwGetKey(this->pWindow, GLFW_KEY_T)) {
		this->isGifting = false;
	}

	Cam.setPosition(mCam.translation());
	Cam.setTarget(currentObjPos);
	Cam.setUp(mCam.up());

    Cam.update();
}

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
	
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}

void Application::end() {
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}

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
}

// https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
Matrix Application::randomTranslation() {
	Matrix m;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> distReal30(-30, 30);
	std::uniform_int_distribution<std::mt19937::result_type> distPositive30(8, 30);

	return Matrix().translation((float)distReal30(rng), (float)distPositive30(rng), (float)distReal30(rng));
}

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

bool Application::checkCollision(BaseModel* sleigh, BaseModel* model_b) {
	AABB bbox_sleigh = sleigh->boundingBox();
	AABB bbox_b = model_b->boundingBox();
	bbox_sleigh = bbox_sleigh.transform(sleigh->transform());
	bool collision = false;

	Vector minB = bbox_b.Min;
	Vector maxB = bbox_b.Max;
	Vector minS = bbox_sleigh.Min;
	Vector maxS = bbox_sleigh.Max;

	// std::cout << "MinH: X = " << minB.X << "\tY = " << minB.Y << "\tZ = " << minB.Z << "/\tMaxH: X = " << maxB.X << "\tY = " << maxB.Y << "\tZ = " << maxB.Z << std::endl;
	// std::cout << "MaxS: X = " << maxS.X << "\tY = " << maxS.Y << "\tZ = " << maxS.Z << "/\tMinS: X = " << minS.X << "\tY = " << minS.Y << "\tZ = " << minS.Z << std::endl;

	if ( (bbox_sleigh.Max.X >= bbox_b.Min.X && bbox_sleigh.Min.X <= bbox_b.Max.X)
		&& (bbox_sleigh.Max.Y >= bbox_b.Min.Y && bbox_sleigh.Min.Y <= bbox_b.Max.Y)
		&& (bbox_sleigh.Max.Z >= bbox_b.Min.Z && bbox_sleigh.Min.Z <= bbox_b.Max.Z)) {
		collision = true;
	}
	return collision;
}

bool Application::checkGiftCollision(BaseModel* sleigh, BaseModel* model_b) {
	AABB bbox_sleigh = sleigh->boundingBox();
	AABB bbox_b = model_b->boundingBox();
	bool collision = false;

	Vector minB = bbox_b.Min;
	Vector maxB = bbox_b.Max;
	Vector minS = bbox_sleigh.Min;
	Vector maxS = bbox_sleigh.Max;

	// std::cout << "MinH: X = " << minB.X << "\tY = " << minB.Y << "\tZ = " << minB.Z << "/\tMaxH: X = " << maxB.X << "\tY = " << maxB.Y << "\tZ = " << maxB.Z << std::endl;
	// std::cout << "MaxS: X = " << maxS.X << "\tY = " << maxS.Y << "\tZ = " << maxS.Z << "/\tMinS: X = " << minS.X << "\tY = " << minS.Y << "\tZ = " << minS.Z << std::endl;

	if ((bbox_sleigh.Max.X >= bbox_b.Min.X && bbox_sleigh.Min.X <= bbox_b.Max.X)
		&& (bbox_sleigh.Max.Y >= bbox_b.Min.Y && bbox_sleigh.Min.Y <= bbox_b.Max.Y)
		&& (bbox_sleigh.Max.Z >= bbox_b.Min.Z && bbox_sleigh.Min.Z <= bbox_b.Max.Z)) {
		collision = true;
	}
	return collision;
}



Vector Application::calc3DRay(float x, float y, Vector& Pos)
{
	Matrix projection = Cam.getProjectionMatrix();
	projection.invert();
	Matrix view = Cam.getViewMatrix();
	view.invert();
	Pos = view.translation();

	// Inverse Projektionsmatrix auf Mauszeigerkoordinate angewandt
	Vector mouse(x, y, 0);
	Vector direction = projection.transformVec4x4(mouse);
	return view.transformVec3x3(direction);
}
