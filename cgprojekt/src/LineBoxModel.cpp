//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel(float Width, float Height, float Depth) {

	VB.begin();
	float x = Width / 2;
	float y = Height / 2;
	float z = Depth / 2;

	Vector one(x, y, z);
	Vector two(x, -y, z);	
	Vector three(-x, -y, z);
	Vector four(-x, y, z);
	
	Vector five(x, y, -z);
	Vector six(x, -y, -z);
	Vector seven(-x, -y, -z);
	Vector eight(-x, y, -z);

	// Viereck: vorne
	VB.addVertex(one);
	VB.addVertex(two);
	VB.addVertex(two);
	VB.addVertex(three);
	VB.addVertex(three);
	VB.addVertex(four);
	VB.addVertex(four);
	VB.addVertex(one);

	// Viereck: hinten
	VB.addVertex(five);
	VB.addVertex(six);
	VB.addVertex(six);
	VB.addVertex(seven);
	VB.addVertex(seven);
	VB.addVertex(eight);
	VB.addVertex(eight);
	VB.addVertex(five);

	// Viereck Verbindung vorne und hinten
	VB.addVertex(one);
	VB.addVertex(five);
	VB.addVertex(two);
	VB.addVertex(six);
	VB.addVertex(three);
	VB.addVertex(seven);
	VB.addVertex(four);
	VB.addVertex(eight);

	VB.end();
}

void LineBoxModel::draw(const BaseCamera& Cam) {

    BaseModel::draw(Cam);
  
	VB.activate();

	glDrawArrays(GL_LINES, 0, VB.vertexCount());

	VB.deactivate();
}
