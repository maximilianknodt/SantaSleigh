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

bool City::loadModels(std::vector<const char*> buildings, int width, int length, int streetWidth) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> random(0, buildings.size() - 1);
	float wDistance = 0.0f;
	float lDistance = 0.0f;
	Model* current;
	for (int i = 0; i < width; i++) {
		wDistance = 0;
		for (int j = 0; j < length; j++) {
			current = new Model();
			current->shader(new PhongShader());
			int randInt = random(rng);
			current->load(buildings.at(randInt), false);

			Building* building = new Building();
			Building::BuildingProperties properties {
				buildings.at(randInt),
				ASSET_DIRECTORY "Gold_Star.obj",
				Matrix().translation(wDistance, 0, lDistance),
				randomBool()
			};
			building->loadModels(properties);
			models.push_back(building);
			if (properties.target) targets.push_back(building);

			std::cout << "X: " << wDistance << std::endl;
			std::cout << "Z: " << lDistance << std::endl;
			wDistance += current->boundingBox().size().X + streetWidth;
		}
		lDistance += 10;
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

void City::removeTarget(Building* target) {
	this->targets.remove(target);
}

bool City::randomBool() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> genBool(0, 1);
	return genBool(rng);
}