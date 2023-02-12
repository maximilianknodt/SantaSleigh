#pragma once
#ifndef Star_hpp
#define Star_hpp

#include "Model.h"

class Star : public BaseModel {
public:
	struct Movement {
		Movement() {}
		Movement(float A, float F, float T): Amplitude(A), Frequency(F), Time(T) {}
		float Amplitude;
		float Frequency;
		float Time;
	};

	Star();
	virtual ~Star();
	bool loadModel(const char* star);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);
	void setMovement(const float& f, const float& a) {
		this->Movement.Frequency = f;
		this->Movement.Amplitude = a;
	}
	Model* star;
protected:
	Movement Movement;
	void applyShaderParameter();
};

#endif