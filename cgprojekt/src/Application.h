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
#include "LineBoxModel.h"
#include "City.h"
#include "Text.h"

class Application {
public:
    typedef std::list<BaseModel*> ModelList;
    typedef std::list<BaseModel*> StarList;
    Application(GLFWwindow* pWin, float wWidth, float wHeight);
    void start();
    void update(float dtime);
    void draw();
    void end();
    void showTexts();
    void showPoints();
    void showKeyBindings();
    void showPointDeduction();

protected:
	void createSantaWorld();
    void keyboardInput(float& xRot, float& yRot, float& zRot, bool& drive);
    bool checkCollision(BaseModel* model_a, BaseModel* model_b);
    bool checkGiftCollision(BaseModel* sleigh, BaseModel* model_b);
    void drawBoundingBox(AABB box);
    Matrix randomTranslation();

    bool isGifting;
    float pGiftTravel;
    int points;
    float windowWidth;
    float windowHeight;
    double startTime;
    double deductionTime;
    bool option;
    bool pointDeduction;

    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
	BaseModel* pModel;
	ShadowMapGenerator ShadowGenerator;
    SantaSleigh* pSantaSleigh;
    LineBoxModel* pSleighBox;
    StarList pStars;
    City* pCity; 
    Model* pGift;
    Model* pGround;
    Model* pSky;
    Text text;
    Text textDied;
};

#endif /* Application_hpp */
