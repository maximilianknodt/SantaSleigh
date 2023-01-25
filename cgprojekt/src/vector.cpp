#include "vector.h"
#include <assert.h>
#include <math.h>

#define EPSILON 1e-6

Vector::Vector(float x, float y, float z) : X(x), Y(y), Z(z) {}

Vector::Vector(float f) : X(f), Y(f), Z(f) {}

Vector::Vector() {}

/**
 * Berechnung des Skalarpodukts
 * (Wenn = 0.0, dann sind beide Vektoren orthogonal zueinander)
*/
float Vector::dot(const Vector& v) const {
	float result = 0.0f;
	result += this->X * v.X;
	result += this->Y * v.Y;
	result += this->Z * v.Z;
	return result;
}

/**
 * Berechnung des Kreuzprodukts
 * (Ergebnisvektor steht orthogonal zu beiden Ausgangsvektoren)
*/
Vector Vector::cross(const Vector& v) const {
	Vector result;
	result.X = (this->Y * v.Z - this->Z * v.Y);
	result.Y = (this->Z * v.X - this->X * v.Z);
	result.Z = (this->X * v.Y - this->Y * v.X);
	return result;
}


// ------------------- Operatorenueberladungen ------------------- 
/**
 * Addition zweier Vektoren
*/
Vector Vector::operator+(const Vector& v) const {
	Vector result;
	result.X = this->X + v.X;
	result.Y = this->Y + v.Y;
	result.Z = this->Z + v.Z;
	return result;
}

/**
 * Subtraktion zweier Vektoren
*/
Vector Vector::operator-(const Vector& v) const {
	Vector result;
	result.X = this->X - v.X;
	result.Y = this->Y - v.Y;
	result.Z = this->Z - v.Z;
	return result;
}

/**
 * Multiplikation eines Vektors mit einer Zahl
*/
Vector Vector::operator*(float c) const {
	Vector result;
	result.X = this->X * c;
	result.Y = this->Y * c;
	result.Z = this->Z * c;
	return result;
}

/**
 * TODO
*/
Vector Vector::operator-() const {
	Vector result;
	result = *this * -1;
	return result;
}

/**
 * Addition zweier Vektoren
 * Ergebnis steht im Objekt, auf das dieser Operator aufgerufen wird
 * return: *this
*/
Vector& Vector::operator+=(const Vector& v) {
	Vector tmp = *this + v;
	*this = tmp;
	return *this;
}


/**
 * Normierung, also Berechnung des Einheitsvektors
*/
Vector& Vector::normalize() {
	Vector tmp = *this * (1.0f / this->length());
	*this = tmp;
	return *this;
}

/**
 * Berechnung der Laenge des Vektors
*/
float Vector::length() const {
	return sqrt(lengthSquared());
}

/**
 * Berechnung der quadratischen Laenge des Vektors unter Verwendung des Skalarprodukts
*/
float Vector::lengthSquared() const {
	return this->dot(*this);
}

Vector Vector::reflection(const Vector& normal) const {
	Vector reflect = *this - normal * 2 * (this->dot(normal));
	return reflect;
}

bool Vector::triangleIntersection( const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const {
    // Normale der Ebenen berechnen
    Vector n = (b - a).cross(c - a);
    n.normalize();

    // Auf Parallelität 
    if (d.dot(n) == 0) return false;

    // Abstand d zur Ebene berechnen
    float dist = n.dot(a);

    // s berechnen
    s = (dist - (n.dot(*this))) / (n.dot(d));
    if (s < 0) return false;

    // Strahl berechnen
    Vector p = (*this) + d * s;

    // Auf Schnittpunkt überprüfen
    float abc = ((b - a).cross(c - a)).length() / 2;
    float abp = ((b - a).cross(p - a)).length() / 2;
    float acp = ((c - a).cross(p - a)).length() / 2;
    float bcp = ((c - b).cross(p - b)).length() / 2;    
    return abc + EPSILON >= (abp + acp + bcp); 
}