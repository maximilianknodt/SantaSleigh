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
/// <summary>
/// Methode zum Laden der Stadt
/// </summary>
/// <param name="buildings">Vector mit Pfaden zu Geb‰udeobjekten</param>
/// <param name="width">Anzahl der Geb‰ude in der Breite</param>
/// <param name="length">Anzahl der Geb‰ude in der L‰nge</param>
/// <param name="streetWidth">"Straﬂenbreite", also Abstand zwischen Geb‰uden</param>
/// <returns></returns>
bool City::loadModels(std::vector<const char*> buildings, int width, int length, int streetWidth) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> random(0, buildings.size() - 1);
	float wDistance = 0.0f;
	float lDistance = 0.0f;
	Model* current;
	for (int i = 0; i < width; i++) {
		wDistance = 0;
		float lPrefix = 1;
		if (i % 2 != 0) { lPrefix = -1; };
		for (int j = 0; j < length; j++) {
			float wPrefix = 1;
			if (j % 2 != 0) { wPrefix = -1; };
			current = new Model();
			current->shader(new PhongShader());
			int randInt = random(rng);
			current->load(buildings.at(randInt), false);

			Building* building = new Building();
			Building::BuildingProperties properties {
				buildings.at(randInt),
				ASSET_DIRECTORY "Gold_Star.obj",
				Matrix().scale(2.0f).translation(wDistance * wPrefix, 0, lDistance * lPrefix),
				randomBool()
			};
			building->loadModels(properties);
			models.push_back(building);
			if (properties.target) targets.push_back(building);

			if (j % 2 == 0) {
				wDistance += current->boundingBox().size().X + streetWidth;
			}
		}
		if (i % 2 == 0) {
			lDistance += streetWidth;
		}
	}
	return true;
}

/// <summary>
/// Aktualisiert alle Geb‰ude in der Stadt.
/// </summary>
/// <param name="dtime">Deltatime</param>
void City::update(float dtime) {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Building* model = *it;
		model->update(dtime);
	}
}

/// <summary>
/// Zeichnet alle Geb‰ude der Stadt.
/// </summary>
/// <param name="cam">Kameraobjekt</param>
void City::draw(const BaseCamera& cam) {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Building* model = *it;
		model->draw(cam);
	}
}

/// <summary>
/// Entfernt ein Targetgeb‰ude aus der Targetliste.
/// </summary>
/// <param name="target"></param>
void City::removeTarget(Building* target) {
	this->targets.remove(target);
}

/// <summary>
/// Generiert einen zuf‰lligen boolschen Wert.
/// </summary>
/// <returns></returns>
bool City::randomBool() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> genBool(0, 1);
	return genBool(rng);
}