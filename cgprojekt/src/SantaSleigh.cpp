#include "SantaSleigh.h"
#include "PhongShader.h"

SantaSleigh::SantaSleigh() {
	this->sleigh = new Model();
	this->deer = new Model();

	this->leftRight = 0;
	this->upDown = 0;
	this->shift = 0;
}

SantaSleigh::~SantaSleigh() {
	delete this->sleigh;
	delete this->deer;
}

bool SantaSleigh::loadModels(const char* sleigh, const char* deer) {
	this->deer->shader(new PhongShader(), true);
	this->deer->load(deer, true);

	this->sleigh->shader(new PhongShader(), true);
	this->sleigh->load(sleigh, true);
	
	return true;
}

void SantaSleigh::steer(float upDown, float leftRight, float shift) {
	this->upDown = upDown;
	this->leftRight = leftRight;
	this->shift = shift;
}

void SantaSleigh::update(float dtime) {
	Matrix mForward, mLeftRight, mUpDown, mShift;

	mForward.translation(0.0, 0.0, 1 * dtime);
	mUpDown.rotationX(this->upDown * dtime);
	mLeftRight.rotationY(this->leftRight * dtime);
	mShift.rotationZ(this->shift * dtime);

	Matrix matrix = mForward * mShift * mLeftRight * mUpDown;

	this->transform(transform() * matrix);

	this->deer->transform(transform());
	this->sleigh->transform(transform());
}

void SantaSleigh::draw(const BaseCamera& cam) {
	this->sleigh->draw(cam);
	this->deer->draw(cam);
}