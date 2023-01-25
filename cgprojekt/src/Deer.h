#ifndef Deer_hpp
#define Deer_hpp

#include "Model.h"

class Deer : public BaseModel {
public:
	Deer();
	virtual ~Deer();
	bool loadModel(const char* deer);
	void steer(float leftRight, float upDown, float shift);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

protected:
	Model* deer;
	float leftRight;
	float upDown;
	float shift;
};

#endif