#include "Building.h"
#include "PhongShader.h"
#include <iostream>
#include <random>
#include <cmath>

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif
#define PI 3.14

Building::Building() {
	this->building = new Model();
	this->totalTime = 0.0f;
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
		this->star = new Model();
		this->star->shader(new PhongShader, true);
		this->star->load(properties.fStar);
		this->star->transform(properties.position);
		this->star->transform(this->star->transform() * Matrix().translation(0, height + 4, 0));
		this->star->transform(this->star->transform() * Matrix().scale(0.10));
	}
	return true;
}

void Building::removeTarget() {
	properties.target = false;
}

void Building::update(float dtime) {
	if (this->properties.target) {
		this->totalTime += dtime;
		float amplitude = 0.02f;
		float frequency = 1.0f;
		float upDown = 0.0f;
		upDown = amplitude * sin(PI * frequency * totalTime);
		this->star->transform(this->star->transform() * Matrix().translation(0.0f, upDown, 0.0f));
	}
}

void Building::draw(const BaseCamera& cam) {
	this->building->draw(cam);
	if(properties.target) this->star->draw(cam);
}