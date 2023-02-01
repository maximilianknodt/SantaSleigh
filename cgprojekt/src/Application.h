//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "ShadowMapGenerator.h"
#include "SantaSleigh.h"
#include "Sleigh.h"
#include "Deer.h"

class Application {
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
protected:
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
    void keyboardInput(float& xRot, float& yRot, float& zRot);
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
	BaseModel* pModel;
	ShadowMapGenerator ShadowGenerator;
    Sleigh* pSleigh;
    Deer* pDeer;
    SantaSleigh* pSantaSleigh;
};

#endif /* Application_hpp */
