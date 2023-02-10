#include "City.h"
#include "PhongShader.h"

City::City() {
}

City::~City() {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Model* model = *it;
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
			current = new Model();
			current->shader(new PhongShader());
			current->load(building, false);
			Matrix m;
			m.translation(i * wDistance, 0, j * lDistance);
			current->transform(m);
			current->transformBoundingBox(m);
			models.push_back(current);
		}
	}
	return true;
}

void City::reset() {
}

void City::update(float dtime) {
}

void City::draw(const BaseCamera& cam) {
	for (ModelList::iterator it = models.begin(); it != models.end(); it++) {
		Model* model = *it;
		model->draw(cam);
	}
}