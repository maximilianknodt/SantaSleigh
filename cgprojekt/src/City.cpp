#include "City.h"
#include "PhongShader.h"
#include <iostream>
#include <random>
#include "Building.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

City::City() {
}

City::~City() {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Building* model = *it;
		delete model;
	}
}

bool City::loadModels(const char* building, int width, int length, int streetWidth) {
	Model* current = new Model();
	current->shader(new PhongShader());
	current->load(building, false);
	float wDistance = current->boundingBox().Max.X - current->boundingBox().Min.X + streetWidth;
	float lDistance = current->boundingBox().Max.Z - current->boundingBox().Min.Z + streetWidth;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < length; j++) {
			Building* building = new Building();
			Building::BuildingProperties properties {
				ASSET_DIRECTORY "3d-model.obj",
				ASSET_DIRECTORY "Gold_Star.obj",
				Matrix().translation(i * wDistance, 0, j * lDistance),
				randomBool()
			};
			building->loadModels(properties);
			models.push_back(building);
		}
	}
	return true;
}

void City::reset() {
}

void City::update(float dtime) {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Building* model = *it;
		model->update(dtime);
	}
}

void City::draw(const BaseCamera& cam) {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Building* model = *it;
		model->draw(cam);
	}
}

bool City::randomBool() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> genBool(0, 1);
	return genBool(rng);
}