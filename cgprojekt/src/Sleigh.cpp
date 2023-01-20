#include "Sleigh.h"
#include "PhongShader.h"

Sleigh::Sleigh() {
	this->sleigh = new Model();
	this->leftRight = 0;
	this->upDown = 0;
	this->shift = 0;
}

Sleigh::~Sleigh() {
	delete this->sleigh;
}

bool Sleigh::loadModel(const char* santaSleigh) {
	this->sleigh->shader(new PhongShader(), true);
	this->sleigh->load(santaSleigh, true);
	this->transform(Matrix().scale(0.02));
	return true;
}

void Sleigh::steer(float leftRight, float upDown, float shift) {
	this->leftRight = leftRight;
	this->upDown = upDown;
	this->shift = shift;
}

void Sleigh::update(float dtime) {
	Matrix mForward, mLeftRight, mUpDown, mShift;

	this->transform(transform());

	mForward.translation(0.0, 0.0, 0);
	mLeftRight.rotationY(this->leftRight * dtime);
	mUpDown.rotationX(this->upDown * dtime);
	mShift.rotationZ(this->shift * dtime);

	Matrix matrix = transform() * mForward * mLeftRight * mUpDown * mShift;

	this->transform(matrix);
	this->sleigh->transform(transform());
}

void Sleigh::draw(const BaseCamera& cam) {
	this->sleigh->draw(cam);
}