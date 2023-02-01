#ifndef SantaSleigh_hpp
#define SantaSleigh_hpp

#include "Model.h"

class SantaSleigh : public BaseModel {
public:
	SantaSleigh();
	virtual ~SantaSleigh();
	bool loadModels(const char* deer, const char* sleigh);
	void steer(float leftRight, float upDown, float shift);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

	Matrix getDeerTransform();
	Matrix getLastTransform();

	Vector getPosition();
	Vector getLastPosition();
	void setLastPosition(const Vector lastPos);

protected:
	Model* deer;
	Model* sleigh;
	
	Matrix lastTransform;
	Vector lastPos;
	
	float leftRight;
	float upDown;
	float shift;
};

#endif