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

bool Sleigh::loadModel(const char* sleigh) {
	this->sleigh->shader(new PhongShader(), true);
	this->sleigh->load(sleigh, true);

	this->transform(this->sleigh->transform());

	return true;
}

void Sleigh::steer(float leftRight, float upDown, float shift) {
	this->leftRight = leftRight;
	this->upDown = upDown;
	this->shift = shift;
}

void Sleigh::update(float dtime, const Matrix mDeer) {
	Matrix mForward, mLeftRight, mUpDown, mShift;

	mForward.translation(0.0, 0.0, 0 * dtime);
	mUpDown.rotationX(this->upDown * dtime);
	mLeftRight.rotationY(this->leftRight * dtime);
	mShift.rotationZ(this->shift * dtime);

	Matrix matrix = mForward * mShift * mLeftRight * mUpDown;

	this->transform(mDeer);

	this->sleigh->transform(transform());
}

void Sleigh::draw(const BaseCamera& cam) {
	this->sleigh->draw(cam);
}