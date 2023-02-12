#include "Building.h"
#include "PhongShader.h"
#include "StarShader.h"
#include <iostream>
#include <random>
#include "Star.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Building::Building() {
	this->building = new Model();
	this->star = new Star();
}

Building::~Building() {
	delete this->building;
	delete this->star;
}

bool Building::loadModels(BuildingProperties properties)
{
	this->properties = properties;
	this->building->shader(new PhongShader, true);
	this->building->load(properties.fBuilding, true);
	this->building->transform(this->building->transform() * properties.position);
	this->building->transformBoundingBox(properties.position);

	if (properties.target) {
		float height = this->building->boundingBox().size().Y;
		this->star->loadModel(properties.fStar);
		this->star->star->transform(Matrix().translation(0, height + 2, 0));
		this->star->star->transform(this->star->transform() * Matrix().scale(0.5));
		this->star->setMovement(1.0f, 2.0f);
	}
	return true;
}


void Building::update(float dtime) {
	if (properties.target) this->star->update(dtime);
}

void Building::draw(const BaseCamera& cam) {
	this->building->draw(cam);
	if(properties.target) this->star->draw(cam);
}