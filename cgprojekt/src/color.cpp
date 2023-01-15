#include "color.h"
#include <assert.h>

Color::Color() : R(0), G(0), B(0) {}			// 1. Setzte Farbe F auf Schwarz

Color::Color(float r, float g, float b) : R(r), G(g), B(b) {}

/**
 * Mutiplikation zweier RGB Farben
*/
Color Color::operator*(const Color& c) const {
	Color result;
	result.R = this ->R * c.R;
	result.G = this ->G * c.G;
	result.B = this ->B * c.B;
    return result;
}

/**
 * Mutiplikation einer RGB Farbe mit einer Variablen
*/
Color Color::operator*(const float Factor) const {
	Color result;
	result.R = this->R * Factor;
	result.G = this->G * Factor;
	result.B = this->B * Factor;
	return result;
}

/**
 * Addition zweier RGB Farben
*/
Color Color::operator+(const Color& c) const {
	Color result;
	// Frage: Warum nicht durch 2 teilen?
	result.R = this->R + c.R;
	result.G = this->G + c.G;
	result.B = this->B + c.B;
	return result;
}

/**
 * Mutiplikation zweier RGB Farben
 * return *this
*/
Color& Color::operator+=(const Color& c) {
	Color tmp = *this + c;
	*this = tmp;
	return *this;
}