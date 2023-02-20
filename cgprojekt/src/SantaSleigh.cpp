#include "SantaSleigh.h"
#include "PhongShader.h"

SantaSleigh::SantaSleigh() {
	this->sleigh = new Model();
	this->deer = new Model();
	this->lastPos = Vector(0, 0, 0);
	this->lastForward = this->transform().forward();
	this->startPos = Vector(0, 50, 0);

	this->leftRight = 0;
	this->upDown = 0;
	this->shift = 0;
	this->speed = 0;
	this->maxSpeed = 80.0;
	this->acceleration = 10.0;
}

/// <summary>
/// Loescht die Modelle
/// </summary>
SantaSleigh::~SantaSleigh() {
	delete this->sleigh;
	delete this->deer;
}

/// <summary>
/// Laedt die beiden Modelle und fuegt den Shader hinzu.
/// </summary>
/// <param name="sleigh">Dateipfad</param>
/// <param name="deer">Dateipfad</param>
/// <returns>True, wenn Laden erfolreich war.</returns>
bool SantaSleigh::loadModels(const char* sleigh, const char* deer) {
	this->deer->shader(new PhongShader(), true);
	this->deer->load(deer, true);

	this->sleigh->shader(new PhongShader(), true);
	this->sleigh->load(sleigh, true);

	this->lastPos = this->transform().translation();
	
	return true;
}

/// <summary>
/// Setzt die Werte fuer die Stuerungsbewegungen.
/// </summary>
/// <param name="upDown">Pitch</param>
/// <param name="leftRight">Yaw</param>
/// <param name="shift">Roll</param>
/// <param name="drive">in Bewegung</param>
void SantaSleigh::steer(float upDown, float leftRight, float shift, bool drive) {
	this->upDown = upDown;
	this->leftRight = leftRight;
	this->shift = shift;
	this->drive = drive;
}

/// <summary>
/// Setzt den SantaSleigh an den Ursprung zurueck.
/// </summary>
void SantaSleigh::reset() {
	this->transform(Matrix().translation(this->startPos));
	this->setSpeed(0.0f);
}

/// <summary>
/// Transformiert die Modelmatrizen anhand der Steuerbewegungen.
/// </summary>
/// <param name="dtime">Zeit zwischen zwei Frames</param>
void SantaSleigh::update(float dtime) {
	Matrix mYPR, mForward, matrix;

	mYPR.rotationYawPitchRoll(leftRight * dtime, upDown * dtime, shift * dtime);

	Vector forward = this->transform().forward().normalize();
	this->upgradeSpeed(forward.Y, dtime);

	mForward.translation(0.0, 0.0, this->speed * dtime);
	matrix = mForward * mYPR;

	this->transform(transform() * matrix);

	this->deer->transform(transform());
	this->sleigh->transform(transform());
}

/// <summary>
/// Veraendert die Bewegungsgeschwindigkeit des Schlittens anhand der vertikalen Ausrichtung.
/// </summary>
/// <param name="forwardY">Ausrichtungswert</param>
/// <param name="dtime">Zeit zwischen zwei Frames</param>
void SantaSleigh::upgradeSpeed(float forwardY, float dtime) {
	float gravity = this->fakeGravityInfluence(forwardY);

	if (this->drive == true) {
		if (this->speed < this->maxSpeed) {
			this->speed += (this->acceleration + gravity) * dtime;
		}
	}
	else {
		if (this->speed > 0) {
			this->speed -= (this->acceleration + gravity) * dtime;
		}
	}
}

/// <summary>
/// Ermittelt einen "Gravitationseinfluss" je nach vertikaler Ausrichtung.
/// </summary>
/// <param name="forwardY">Ausrichtungswert</param>
/// <returns></returns>
float SantaSleigh::fakeGravityInfluence(float forwardY) {
	float gravity = 0.0;
	float fy = abs(forwardY);

	if (fy > 0.0) {
		if (fy <= 0.25) gravity = 0.75;
		else if (fy > 0.25 && fy < 0.5) gravity = 1.5;
		else if (fy > 0.5 && fy < 0.75) gravity = 2.25;
		else if (fy > 0.75 && fy <= 1.0) gravity = 3.5;
	}
	if (forwardY > 0) gravity *= (-1);

	return gravity;
}

/// <summary>
/// Zeichnet die Modelle
/// </summary>
/// <param name="cam">BaseCamera</param>
void SantaSleigh::draw(const BaseCamera& cam) {
	this->sleigh->draw(cam);
	this->deer->draw(cam);
}

/// <summary>
/// Gibt die Position zurueck.
/// </summary>
/// <returns>Position.</returns>
Vector SantaSleigh::getPosition() {
	return this->transform().translation();
}