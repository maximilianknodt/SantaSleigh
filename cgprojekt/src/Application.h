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
#include "Text.h"

class Application {
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
    void showText();

protected:
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
    void keyboardActivity(float& xRot, float& yRot, float& zRot);
    
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
	BaseModel* pModel;
	ShadowMapGenerator ShadowGenerator;
    Sleigh* pSleigh;
    Deer* pDeer;
    SantaSleigh* pSantaSleigh;
    Text text;

    int points;
};

#endif /* Application_hpp */
