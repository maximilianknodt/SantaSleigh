#include "Deer.h"
#include "PhongShader.h"

Deer::Deer() {
	this->deer = new Model();

	this->leftRight = 0;
	this->upDown = 0;
	this->shift = 0;
}

Deer::~Deer() {
	delete this->deer;
}

bool Deer::loadModel(const char* deer) {
	this->deer->shader(new PhongShader(), true);
	this->deer->load(deer, true);

	this->deer->transform(Matrix().translation(0, 0, 11) * this->deer->transform());

	this->transform(this->deer->transform());

	return true;
}

void Deer::steer(float leftRight, float upDown, float shift) {
	this->leftRight = leftRight;
	this->upDown = upDown;
	this->shift = shift;
}

void Deer::update(float dtime) {
	Matrix mForward, mLeftRight, mUpDown, mShift;

	mForward.translation(0.0, 0.0, 0 * dtime);
	mUpDown.rotationX(this->upDown * dtime);
	mLeftRight.rotationY(this->leftRight * dtime);
	mShift.rotationZ(this->shift * dtime);

	Matrix matrix = mForward * mShift * mLeftRight * mUpDown;

	this->transform(transform() * matrix);

	this->deer->transform(transform());
}

void Deer::draw(const BaseCamera& cam) {
	this->deer->draw(cam);
}