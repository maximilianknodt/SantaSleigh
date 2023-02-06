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
	bool drive = false;
	this->keyboardInput(upDown, leftRight, shift, drive);

	this->pSantaSleigh->steer(upDown, leftRight, shift, drive);
	this->pSantaSleigh->update(dtime);


	Vector v = pSantaSleigh->transform().translation();

	// Laenge der Objektbewegung berechnen und auf Cam-Position anwenden
	float smoothness = 1;
	Vector currentObjPos, lastObjPos, travelDist, camPos;
	currentObjPos = this->pSantaSleigh->getPosition();
	lastObjPos = this->pSantaSleigh->getLastPosition();
	travelDist = currentObjPos - lastObjPos;
	camPos = Cam.position() + (travelDist * smoothness);
	// Letzte Position des Objekts setzen
	this->pSantaSleigh->setLastPosition(currentObjPos);

	// Folgt, aber Wellenbewegung
	Matrix m, n;
	n.translation(Vector(0, 4, -10));
	m = pSantaSleigh->transform() * n;

	Vector objCam = pSantaSleigh->transform().forward();
	
	Cam.setPosition(m.translation());
	Cam.setTarget(currentObjPos);
	Cam.setUp(m.up());


	Vector forw = Cam.target() - Cam.position();
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
void Application::keyboardInput(float& xRot, float& yRot, float& zRot, bool& drive) {
	if (glfwGetKey(this->pWindow, GLFW_KEY_W)) { xRot = 1; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_S)) { xRot = -1; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_Q)) { yRot = 1; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_E)) { yRot = -1; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_D)) { zRot = 1; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_A)) { zRot = -1; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_SPACE)) {
		drive = true;
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