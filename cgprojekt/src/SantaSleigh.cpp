#include "SantaSleigh.h"
#include "PhongShader.h"

SantaSleigh::SantaSleigh() {
	this->sleigh = new Model();
	this->deer = new Model();
	this->lastPos = Vector(0, 0, 0);
	this->lastForward = this->transform().forward();
	this->startPos = Vector(0, 50, 0);

	this->leftRight = 0;
	this->upDown = 0;
	this->shift = 0;
	this->speed = 0;
	this->maxSpeed = 80.0;
	this->acceleration = 10.0;
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

void SantaSleigh::steer(float upDown, float leftRight, float shift, bool drive) {
	this->upDown = upDown;
	this->leftRight = leftRight;
	this->shift = shift;
	this->drive = drive;
}

void SantaSleigh::reset() {
	this->transform(Matrix().translation(this->startPos));
	this->setSpeed(0.0f);
}

void SantaSleigh::update(float dtime) {
	Matrix mYPR, mForward, matrix;

	mYPR.rotationYawPitchRoll(leftRight * dtime, upDown * dtime, shift * dtime);

	Vector forward = this->transform().forward().normalize();
	this->upgradeSpeed(forward.Y, dtime);

	mForward.translation(0.0, 0.0, this->speed * dtime);
	matrix = mForward * mYPR;

	this->transform(transform() * matrix);

	this->deer->transform(transform());
	this->sleigh->transform(transform());
}

void SantaSleigh::upgradeSpeed(float forwardY, float dtime) {
	float gravity = this->fakeGravityInfluence(forwardY);

	if (this->drive == true) {
		if (this->speed < this->maxSpeed) {
			this->speed += (this->acceleration + gravity) * dtime;
		}
	}
	else {
		if (this->speed > 0) {
			this->speed -= (this->acceleration + gravity) * dtime;
		}
	}
}

float SantaSleigh::fakeGravityInfluence(float forwardY) {
	float gravity = 0.0;
	float fy = abs(forwardY);

	if (fy > 0.0) {
		if (fy <= 0.25) gravity = 0.75;
		else if (fy > 0.25 && fy < 0.5) gravity = 1.5;
		else if (fy > 0.5 && fy < 0.75) gravity = 2.25;
		else if (fy > 0.75 && fy <= 1.0) gravity = 3.5;
	}
	if (forwardY > 0) gravity *= (-1);

	return gravity;
}

void SantaSleigh::draw(const BaseCamera& cam) {
	this->sleigh->draw(cam);
	this->deer->draw(cam);
}

// ggf. fuer weichere Camerabewegung nutzbar
Vector SantaSleigh::getPosition() {
	return this->transform().translation();
}

Vector SantaSleigh::getLastPosition() {
	return this->lastPos;
}

void SantaSleigh::setLastPosition(const Vector lastPos) {
	this->lastPos = lastPos;
}