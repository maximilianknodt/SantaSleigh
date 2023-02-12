#include "Star.h"
#include "StarShader.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Star::Star() {
	this->star = new Model();
}

Star::~Star() {
	delete this->star;
}

bool Star::loadModel(const char* star) {
	StarShader* starShader = new StarShader(ASSET_DIRECTORY);
	this->star->shader(starShader, true);
	this->star->load(star, true);
	return true;
}

void Star::update(float dtime) {
	this->Movement.Time = dtime;
}

void Star::draw(const BaseCamera& cam) {
	applyShaderParameter();
	this->star->draw(cam);
}

void Star::applyShaderParameter() {
	StarShader* Shader = dynamic_cast<StarShader*>(this->star->shader());
	if (!Shader)
		return;
	Shader->setFrequency(this->Movement.Frequency);
	Shader->setAmplitude(this->Movement.Amplitude);
	Shader->setTime(this->Movement.Time);
}