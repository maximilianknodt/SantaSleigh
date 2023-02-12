#pragma once
#ifndef City_hpp
#define City_hpp

#include "Model.h"
#include "Building.h"
#include <list>

class City : public BaseModel {
public:
	City();
	virtual ~City();
	typedef std::list<Building*> ModelList;
	typedef std::list<Model*> TargetList;
	bool loadModels(const char* building, int width, int length, int streetWidth);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);
	ModelList getModels() { return this->models; };
	TargetList* getTargets() { return this->targets; };
	void reset();
protected:
	ModelList models;
	TargetList* targets;
	bool randomBool();
};

#endif