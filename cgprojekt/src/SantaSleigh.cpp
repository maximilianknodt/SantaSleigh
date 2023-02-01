#include "SantaSleigh.h"
#include "PhongShader.h"

SantaSleigh::SantaSleigh() {
	this->sleigh = new Model();
	this->deer = new Model();
	this->lastPos = Vector(0, 0, 0);
	this->lastForward = this->transform().forward();

	this->lastTransform = this->deer->transform();

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

	this->lastPos = this->transform().translation();
	
	return true;
}

void SantaSleigh::steer(float upDown, float leftRight, float shift) {
	this->upDown = upDown;
	this->leftRight = leftRight;
	this->shift = shift;
}

void SantaSleigh::update(float dtime) {
	/*
	Matrix mForward, mLeftRight, mUpDown, mShift;

	mForward.translation(0.0, 0.0, 10 * dtime);
	mUpDown.rotationX(this->upDown * dtime);
	mLeftRight.rotationY(this->leftRight * dtime);
	mShift.rotationZ(this->shift * dtime);
	Matrix matrix = mForward * mShift * mLeftRight * mUpDown;
	*/

	this->lastTransform = this->deer->transform();

	Matrix mYPR, mForward;
	mForward.translation(0.0, 0.0, 5 * dtime);
	mYPR.rotationYawPitchRoll(leftRight * dtime, upDown * dtime, shift * dtime);
	Matrix matrix = mForward * mYPR;

	this->transform(transform() * matrix);

	this->deer->transform(transform());
	this->sleigh->transform(transform());
}

void SantaSleigh::draw(const BaseCamera& cam) {
	this->sleigh->draw(cam);
	this->deer->draw(cam);
}

Vector SantaSleigh::getPosition() {
	return this->transform().translation();
}

Vector SantaSleigh::getLastPosition() {
	return this->lastPos;
}

void SantaSleigh::setLastPosition(const Vector lastPos) {
	this->lastPos = lastPos;
}

Matrix SantaSleigh::getDeerTransform() {
	return this->deer->transform();
}

Matrix SantaSleigh::getLastTransform() {
	return this->lastTransform;
}