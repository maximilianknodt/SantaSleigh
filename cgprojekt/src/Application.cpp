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
#include "Text.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048), text(Cam, 1200, 800) {
	createScene();
	//this->text.loadFont(ASSET_DIRECTORY "fonts/OpenSans-Regular.ttf", 48);
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
	
	//this->text.renderText(Cam, "Testtext zum Anschauen, ob er auch lang genug ist", 0.0f, 0.0f, 1.0f, Color());


	float upDown = 0;		// xRot
	float leftRight = 0;	// yRot
	float shift = 0;		// zRot

	this->keyboardActivity(upDown, leftRight, shift);

	//pDeer->steer(leftRight, upDown, shift);
	//pSleigh->steer(leftRight, upDown, shift);
	
	//pDeer->update(dtime);
	//pSleigh->update(dtime, this->pDeer->transform());

	this->pSantaSleigh->steer(upDown, leftRight, shift);
	this->pSantaSleigh->update(dtime);
	
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
void Application::keyboardActivity(float& xRot, float& yRot, float& zRot) {
	if (glfwGetKey(this->pWindow, GLFW_KEY_W)) { xRot = 1.0f; }
	if (glfwGetKey(this->pWindow, GLFW_KEY_S)) { xRot = -1.0f; }

	if (glfwGetKey(this->pWindow, GLFW_KEY_A)) {
		if (glfwGetKey(this->pWindow, GLFW_KEY_LEFT_SHIFT)) {
			zRot = -2;
		}
		else yRot = 1.0f;
	}
	if (glfwGetKey(this->pWindow, GLFW_KEY_D)) {
		if (glfwGetKey(this->pWindow, GLFW_KEY_LEFT_SHIFT)) {
			zRot = 2;
		}
		else yRot = -1.0f;
	}
}

void Application::createScene() {
	
	std::cout << "Vor Skybox" << std::endl;
	Matrix m, n;

	// Umgebung als Cube: Skybox
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);

	std::cout << "Vor SantaSleigh" << std::endl;

	this->pSantaSleigh = new SantaSleigh();
	this->pSantaSleigh->shader(new PhongShader(), true);
	this->pSantaSleigh->loadModels(
		ASSET_DIRECTORY "deer.obj",
		ASSET_DIRECTORY "santasleigh.obj");
	Models.push_back(pSantaSleigh);

	/*
	this->pSleigh = new Sleigh();
	this->pSleigh->shader(new PhongShader(), true);
	this->pSleigh->loadModel(ASSET_DIRECTORY "santasleigh.obj");
	Models.push_back(pSleigh);	

	this->pDeer = new Deer();
	this->pDeer->shader(new PhongShader(), true);
	this->pDeer->loadModel(ASSET_DIRECTORY "deer.obj");
	Models.push_back(pDeer);
	*/

	// Globale Lichtquelle
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
}