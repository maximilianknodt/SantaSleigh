#pragma once
#ifndef Building_hpp
#define Building_hpp

#include "Model.h"
#include <list>
#include "Star.h"

class Building : public BaseModel {
public:
	struct BuildingProperties {
		BuildingProperties() {}
		BuildingProperties(const char* fBuilding, const char* fStar, Matrix position, bool target) 
		 : fBuilding(fBuilding), fStar(fStar), position(position), target(target) {}
		const char* fBuilding;
		const char* fStar;
		Matrix position;
		bool target;
	};

	Building();
	virtual ~Building();
	bool loadModels(BuildingProperties properties);
	void update(float dtime);
	virtual void draw(const BaseCamera& Cam);

	Model* building;
	Star* star;
	BuildingProperties properties;
};

#endif