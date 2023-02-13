#include "Building.h"
#include "PhongShader.h"
#include <iostream>
#include <random>

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Building::Building() {
	this->building = new Model();
}

Building::~Building() {
	delete this->building;
	delete this->star;
}

bool Building::loadModels(BuildingProperties properties)
{
	this->properties = properties;
	this->building->shader(new PhongShader, false);
	this->building->load(properties.fBuilding, false);
	this->building->transform(properties.position);
	this->building->transformBoundingBox(properties.position);

	if (properties.target) {
		float height = this->building->boundingBox().size().Y;
		std::cout << "Height: " << height << std::endl;
		this->star = new Model();
		this->star->shader(new PhongShader, true);
		this->star->load(properties.fStar);
		this->star->transform(properties.position);
		this->star->transform(this->star->transform() * Matrix().translation(0, height + 2, 0));
		this->star->transform(this->star->transform()* Matrix().scale(0.18));
	}
	return true;
}

void Building::removeTarget() {
	properties.target = false;
}

void Building::update(float dtime) {
	
}

void Building::draw(const BaseCamera& cam) {
	this->building->draw(cam);
	if(properties.target) this->star->draw(cam);
}