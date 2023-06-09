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
/// <summary>
/// Methode zum Laden eines Geb�udes
/// </summary>
/// <param name="properties">Build properties f�r ein Geb�ude</param>
/// <returns></returns>
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
/// <summary>
/// Setzt die property "target" auf false.
/// </summary>
void Building::removeTarget() {
	properties.target = false;
}

/// <summary>
/// Update Methode aktualisiert das Geb�ude. In diesem Fall wird das Targetsymbol animiert.
/// </summary>
/// <param name="dtime">Deltatime</param>
void Building::update(float dtime) {
	if (this->properties.target) {
		this->totalTime += dtime;
		float amplitude = 0.02f;
		float frequency = 1.0f;
		float upDown = 0.0f;
		upDown = amplitude * sin(PI * frequency * totalTime);
		this->star->transform(this->star->transform() * Matrix().translation(0.0f, upDown, 0.0f));
		this->star->transform(this->star->transform() * Matrix().rotationY(1 *dtime));
	}
}
/// <summary>
/// Draw Methode
/// </summary>
/// <param name="cam">Kameraobjekt</param>
void Building::draw(const BaseCamera& cam) {
	this->building->draw(cam);
	if(properties.target) this->star->draw(cam);
}