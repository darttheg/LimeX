#include "Objects/Vec3.h"
#include <sol/sol.hpp>
#include <algorithm>

#define PI 3.14159265358979323846

Vec3::Vec3() : x(0), y(0), z() {}
Vec3::Vec3(float x) : x(x), y(x), z(x) {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3& other) const { return Vec3(getX() + other.getX(), getY() + other.getY(), getZ() + other.getZ()); }
Vec3 Vec3::operator-(const Vec3& other) const { return Vec3(getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ()); }
Vec3 Vec3::operator*(float scalar) const { return Vec3(getX() * scalar, getY() * scalar, getZ() * scalar); }
Vec3 Vec3::operator/(float scalar) const { return Vec3(getX() / scalar, getY() / scalar, getZ() / scalar); }
bool Vec3::operator==(const Vec3& other) const { return getX() == other.getX() && getY() == other.getY() && getZ() == other.getZ(); }

float Vec3::getLength() const {
	return std::sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
}

float Vec3::getLengthSquared() const {
	return getX() * getX() + getY() * getY() + getZ() * getZ();
}

float Vec3::getDistance(const Vec3& other) const {
	float dx = other.getX() - getX();
	float dy = other.getY() - getY();
	float dz = other.getZ() - getZ();
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float Vec3::getDistanceSquared(const Vec3& other) const {
	float dx = other.getX() - getX();
	float dy = other.getY() - getY();
	float dz = other.getZ() - getZ();
	return dx * dx + dy * dy + dz * dz;
}

bool Vec3::isNearlyZero(float epsilon) const {
	return getLengthSquared() <= (epsilon * epsilon);
}

float Vec3::dot(const Vec3& other) const {
	return getX() * other.getX() + getY() * other.getY() + getZ() * other.getZ();
}

Vec3 Vec3::cross(const Vec3& other) const {
	return Vec3(getY() * other.getZ() - getZ() * other.getY(),
				getZ() * other.getX() - getX() * other.getZ(), 
				getX() * other.getY() - getY() * other.getX());
}

float Vec3::angle(const Vec3& other) const {
	float a2 = getLengthSquared();
	float b2 = other.getLengthSquared();
	if (a2 <= 0.0f || b2 <= 0.0f) return 0.0f;

	float denom = std::sqrt(a2 * b2);
	float c = dot(other) / denom;
	c = std::clamp(c, -1.0f, 1.0f);
	return (float)(std::acos(c) * 180.0f / PI);
}

Vec3 Vec3::normalize() const {
	float len = getLength();
	if (len > 0.0f) {
		float inv = 1.0f / len;
		return Vec3(getX() * inv, getY() * inv, getZ() * inv);
	}

	return Vec3();
}

Vec3 Vec3::normalizeRange(float min, float max) const {
	float len = getLength();
	if (len <= 0.0f) return Vec3();

	float clampedLen = std::clamp(len, min, max);
	float scale = clampedLen / len;
	return Vec3(getX() * scale, getY() * scale, getZ() * scale);
}

Vec3 Vec3::clamp(const Vec3& min, const Vec3& max) const {
	return Vec3(std::clamp(getX(), min.getX(), max.getX()),
				std::clamp(getY(), min.getY(), max.getY()),
				std::clamp(getZ(), min.getZ(), max.getZ()));
}

Vec3 Vec3::reflect(const Vec3& dir) const {
	return *this - dir * (2.0f * this->dot(dir));
}

void Object::Vec3Bind::bind(lua_State* ls) {
	sol::state_view view(ls);
	sol::usertype<Vec3> obj = view.new_usertype<Vec3>(
		"Vec3",
		sol::constructors<Vec3(), Vec3(float), Vec3(float, float, float)>(),
		sol::meta_function::type, [](const Vec3&) { return "Vec3"; },

		sol::meta_function::addition, &Vec3::operator+,
		sol::meta_function::subtraction, &Vec3::operator-,
		sol::meta_function::multiplication, &Vec3::operator*,
		sol::meta_function::division, &Vec3::operator/,
		sol::meta_function::equal_to, &Vec3::operator==,

		"x", sol::property(&Vec3::getX, &Vec3::setX),
		"y", sol::property(&Vec3::getY, &Vec3::setY),
		"z", sol::property(&Vec3::getZ, &Vec3::setZ)
	);

	obj[sol::meta_function::to_string] = [](const Vec3& v) {
		return "(" + std::to_string(v.getX()) + ", " + std::to_string(v.getY()) + ", " + std::to_string(v.getZ()) + ")";
		};

	// Object Vec3, A three-dimensional vector object.

	// Field number x
	// Field number y
	// Field number z

	// Constructor
	// Constructor number x, number y, number z
	// Constructor number all

	// Operation Vec3 Vec3 +
	// Operation Vec3 Vec3 -
	// Operation Vec3 number *
	// Operation Vec3 number /
	// Operation boolean Vec3 ==

	// Returns a copy of this vector.
	// Returns Vec3
	obj.set_function("copy", &Vec3::copy);

	// Returns the length of the vector.
	// Returns number
	obj.set_function("length", &Vec3::getLength);

	// Returns the length of the vector save the square root operation.
	// Returns number
	obj.set_function("lengthSqr", &Vec3::getLengthSquared);

	// Returns the distance between two vectors.
	// Params Vec3 other
	// Returns number
	obj.set_function("distance", &Vec3::getDistance);

	// Returns the distance between two vectors squared.
	// Params Vec3 other
	// Returns number
	obj.set_function("distanceSqr", &Vec3::getDistanceSquared);

	// Returns true if the vector is effectively zero.
	// Params number? epsilon
	// Returns boolean
	obj.set_function("isNearlyZero", &Vec3::isNearlyZero);

	// Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
	// Params Vec3 other
	// Returns number
	obj.set_function("dot", &Vec3::dot);

	// Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
	// Params Vec3 other
	// Returns Vec3
	obj.set_function("cross", &Vec3::cross);

	// Measures the angle between vectors in degrees
	// Params Vec3 other
	// Returns number
	obj.set_function("angle", &Vec3::angle);

	// Returns a normalized unit vector.
	// Returns Vec3
	obj.set_function("normalize", &Vec3::normalize);

	// Returns a normalied vector scaled to clamp between numbers min and max.
	// Params number min, number max
	// Returns Vec3
	obj.set_function("normalizeRng", &Vec3::normalizeRange);

	// Returns a clamped vector to vectors min and max.
	// Params Vec3 min, Vec3 max
	// Returns Vec3
	obj.set_function("clamp", &Vec3::clamp);

	// End Object
}