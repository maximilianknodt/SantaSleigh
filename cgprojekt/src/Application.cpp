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


#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048) {
	createScene();

	//createNormalTestScene();
	//createShadowTestScene();
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
	this->keyboardInput(upDown, leftRight, shift);

	this->pSantaSleigh->steer(upDown, leftRight, shift);
	this->pSantaSleigh->update(dtime);


	Vector v = pSantaSleigh->transform().translation();
	//std::cout << "X: " << v.X << "\tY: " << v.Y << "\tZ: " << v.Z << std::endl;


	// Laenge der Objektbewegung berechnen und auf Cam-Position anwenden
	float smoothness = 1;
	Vector currentObjPos, lastObjPos, travelDist, camPos;
	currentObjPos = this->pSantaSleigh->getPosition();
	lastObjPos = this->pSantaSleigh->getLastPosition();
	travelDist = currentObjPos - lastObjPos;
	camPos = Cam.position() + (travelDist * smoothness);
	// Letzte Position des Objekts setzen
	this->pSantaSleigh->setLastPosition(currentObjPos);


	// Versuch der Winkelberechnung
	Matrix lTrans, cTrans;
	lTrans = this->pSantaSleigh->getLastTransform();
	cTrans = this->pSantaSleigh->transform();
	Vector cX, lX, cY, lY, cZ, lZ;
	cX = cTrans.right();
	lX = lTrans.right();
	cY = cTrans.up();
	lY = lTrans.up();
	cZ = cTrans.forward();
	lZ = lTrans.forward();
	float angleX = cX.dot(lX) / (cX.length() * lX.length());
	float angleY = cY.dot(lY) / (cY.length() * lY.length());
	float angleZ = cZ.dot(lZ) / (cZ.length() * lZ.length());
	float dist = (Cam.position() - currentObjPos).length();
	//float seitenlaenge = 2 * dist.length() * sin(angle/2);
	Vector cp;
	cp.X = Cam.position().X + cos(angleX) * dist;
	cp.Y = Cam.position().Y + cos(angleY) * dist;
	cp.Z = Cam.position().Z + cos(angleZ) * dist;
	


	// Folgt, aber Wellenbewegung
	Matrix m, n;
	n.translation(Vector(0, 2, -10));
	m = pSantaSleigh->transform() * n;


	Vector objCam = pSantaSleigh->transform().forward();
	objCam = objCam * dist;
	//std::cout << "X: " << objCam.X << "\tY: " << objCam.Y << "\tZ: " << objCam.Z << std::endl;

	

	Cam.setPosition(camPos);
	Cam.setTarget(currentObjPos);


	Vector forw = Cam.target() - Cam.position();
	//std::cout << "currentObjPos: " << currentObjPos.X << " | " << currentObjPos.Y << " | " << currentObjPos.Z << std::endl;
	//std::cout << "forw: " << forw.X << " | " << forw.Y << " | " << forw.Z << std::endl;


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

/// <summary>
/// Sets the values for the rotation around the three axes
/// and returns them in [xRot], [yRot] and [zRot]
/// </summary>
/// <param name="xRot"></param>
/// <param name="yRot"></param>
/// <param name="zRot"></param>
void Application::keyboardInput(float& xRot, float& yRot, float& zRot) {
	if (glfwGetKey(this->pWindow, GLFW_KEY_W)) { xRot = 1; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_S)) { xRot = -1; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_A)) {
		if (glfwGetKey(this->pWindow, GLFW_KEY_LEFT_SHIFT)) {
			zRot = -1;
		}
		else {
			yRot = 1;
		}
	}
	if (glfwGetKey(this->pWindow, GLFW_KEY_D)) {
		if (glfwGetKey(this->pWindow, GLFW_KEY_LEFT_SHIFT)) {
			zRot = 1;
		}
		else yRot = -1; //(M_PI / 360)
	}
}

void Application::createScene() {
	// Umgebung als Cube: Skybox
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	pModel->transform(pModel->transform() * Matrix().scale(5));
	Models.push_back(pModel);


	pModel = new Model(ASSET_DIRECTORY "city_blocks.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);


	this->pSantaSleigh = new SantaSleigh();
	this->pSantaSleigh->shader(new PhongShader(), true);
	this->pSantaSleigh->loadModels(
		ASSET_DIRECTORY "deer.obj",
		ASSET_DIRECTORY "santasleigh.obj");
	this->pSantaSleigh->transform(
		this->pSantaSleigh->transform()
		* Matrix().translation(0, 50, 0));
	Models.push_back(pSantaSleigh);

	// Globale Lichtquelle
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
}

void Application::createNormalTestScene() {
	Matrix m, n;

	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);


	pModel = new Model(ASSET_DIRECTORY "scene.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(10, 0, -10);
	pModel->transform(m);
	Models.push_back(pModel);


	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);

	Color c = Color(1.0f, 0.7f, 1.0f);
	Vector a = Vector(1, 0, 0.1f);
	float innerradius = 45;
	float outerradius = 60;

	// point lights
	PointLight* pl = new PointLight();
	pl->position(Vector(-1.5, 3, 10));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	// spot lights
	SpotLight* sl = new SpotLight();
	sl->position(Vector(-1.5, 3, 10));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
}

void Application::createShadowTestScene() {
	pModel = new Model(ASSET_DIRECTORY "shadowcube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "bunny.dae", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);
	
	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0, -1, -1));
	dl->color(Color(0.5, 0.5, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	SpotLight* sl = new SpotLight();
	sl->position(Vector(2, 2, 0));
	sl->color(Color(0.5, 0.5, 0.5));
	sl->direction(Vector(-1, -1, 0));
	sl->innerRadius(10);
	sl->outerRadius(13);
	sl->castShadows(true);
	ShaderLightMapper::instance().addLight(sl);
}