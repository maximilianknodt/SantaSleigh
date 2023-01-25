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

protected:
	Model* sleigh;
	Model* deer;
	float leftRight;
	float upDown;
	float shift;
};

#endif