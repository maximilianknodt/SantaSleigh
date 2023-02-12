#pragma once
#ifndef SantaSleigh_hpp
#define SantaSleigh_hpp

#include "Model.h"

class SantaSleigh : public BaseModel {
public:
	SantaSleigh();
	virtual ~SantaSleigh();
	bool loadModels(const char* deer, const char* sleigh);
	void steer(float leftRight, float upDown, float shift, bool drive);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

	// ggf. fuer weichere Camerabewegung nutzbar
	Vector getPosition();
	Vector getLastPosition();
	void setLastPosition(const Vector lastPos);

	void setLastForward(const Vector lastForward) { this->lastForward = lastForward; };
	Vector getLastForward() { return this->lastForward; };

	float getSpeed() { return this->speed; };
	void setSpeed(float newSpeed) { this->speed = newSpeed; };
	void setAcceleration(float newAccel) { this->acceleration = newAccel; };

	float fakeGravityInfluence(float forwardY);
	void upgradeSpeed(float forwardY, float dtime);
	Model* deer;
	Model* sleigh;
	Vector getStartPos() { return this->startPos; };
	void reset();
protected:
	
	Vector lastPos;
	Vector startPos;
	Vector lastForward;

	float leftRight;
	float upDown;
	float shift;

	bool drive;
	float speed;
	float maxSpeed;
	float acceleration;
};

#endif