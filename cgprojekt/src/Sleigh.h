#ifndef Sleigh_hpp
#define Sleigh_hpp

#include "Model.h"

class Sleigh : public BaseModel {
public:
	Sleigh();
	virtual ~Sleigh();
	bool loadModel(const char* santassleigh);
	void steer(float leftRight, float upDown, float shift);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

protected:
	Model* sleigh;
	float leftRight;
	float upDown;
	float shift;
};

#endif