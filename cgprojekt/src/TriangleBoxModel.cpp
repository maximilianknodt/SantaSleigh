//
//  TriangleBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleBoxModel.h"

TriangleBoxModel::TriangleBoxModel(float Width, float Height, float Depth) {

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

	Vector nVorne(0, 0, 1);
	Vector nHinten(0, 0, -1);
	Vector nOben(0, 1, 0);
	Vector nUnten(0, -1, 0);
	Vector nRechts(1, 0, 0);
	Vector nLinks(-1, 0, 0);

	VB.begin();

	VB.addNormal(nVorne);
	VB.addTexcoord0(0, 0);
	VB.addTexcoord0(1, 0);
	VB.addTexcoord0(1, 1);
	VB.addTexcoord0(0, 1);
	VB.addVertex(four);
	VB.addVertex(one);
	VB.addVertex(two);
	VB.addVertex(three);

	VB.addNormal(nHinten);
	VB.addTexcoord0(0, 0);
	VB.addTexcoord0(1, 0);
	VB.addTexcoord0(1, 1);
	VB.addTexcoord0(0, 1);
	VB.addVertex(five);
	VB.addVertex(eight);
	VB.addVertex(seven);
	VB.addVertex(six);

	VB.addNormal(nOben);
	VB.addTexcoord0(0, 0);
	VB.addTexcoord0(1, 0);
	VB.addTexcoord0(1, 1);
	VB.addTexcoord0(0, 1);
	VB.addVertex(eight);
	VB.addVertex(five);
	VB.addVertex(one);
	VB.addVertex(four);

	VB.addNormal(nUnten);
	VB.addTexcoord0(0, 0);
	VB.addTexcoord0(1, 0);
	VB.addTexcoord0(1, 1);
	VB.addTexcoord0(0, 1);
	VB.addVertex(three);
	VB.addVertex(two);
	VB.addVertex(six);
	VB.addVertex(seven);

	VB.addNormal(nRechts);
	VB.addTexcoord0(0, 0);
	VB.addTexcoord0(1, 0);
	VB.addTexcoord0(1, 1);
	VB.addTexcoord0(0, 1);
	VB.addVertex(one);
	VB.addVertex(five);
	VB.addVertex(six);
	VB.addVertex(two);

	VB.addNormal(nLinks);
	VB.addTexcoord0(0, 0);
	VB.addTexcoord0(1, 0);
	VB.addTexcoord0(1, 1);
	VB.addTexcoord0(0, 1);
	VB.addVertex(eight);
	VB.addVertex(four);
	VB.addVertex(three);
	VB.addVertex(seven);

	VB.end();

	IB.begin();

	// N[] = NV, NV, NV, NV, NH, NH, NH, NH, ...
	// T[] = T1, T2, T3, T4, T1, T2, T3, T4, ...
	// V[] = V1, V2, V3, V4, V5, V6, V7, V8, ...

	for (size_t i = 0; i < 6 * 4; i += 4) {
		IB.addIndex(0 + i);		// Dreieck 1 der Flaeche
		IB.addIndex(1 + i);		// Dreieck 1 der Flaeche
		IB.addIndex(2 + i);		// Dreieck 1 der Flaeche
			
		IB.addIndex(0 + i);		// Dreieck 2 der Flaeche
		IB.addIndex(2 + i);		// Dreieck 2 der Flaeche
		IB.addIndex(3 + i);		// Dreieck 2 der Flaeche
	}

	IB.end();
}

void TriangleBoxModel::draw(const BaseCamera& Cam) {
    BaseModel::draw(Cam);
    
	VB.activate();
	IB.activate();

	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);

	IB.deactivate();
	VB.deactivate();
}
