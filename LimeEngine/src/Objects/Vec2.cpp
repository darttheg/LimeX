#include "Objects/Vec2.h"
#include "Application.h"

#define PI 3.14159265358979323846

Vec2::Vec2() : x(0), y(0) {}
Vec2::Vec2(float x, float y) : x(x), y(y) {}
Vec2::Vec2(float x) : x(x), y(x) {}

Vec2 Vec2::operator+(const Vec2& other) const { return Vec2(getX() + other.getX(), getY() + other.getY()); }
Vec2 Vec2::operator-(const Vec2& other) const { return Vec2(getX() - other.getX(), getY() - other.getY()); }
Vec2 Vec2::operator*(float scalar) const { return Vec2(getX() * scalar, getY() * scalar); }
Vec2 Vec2::operator/(float scalar) const { return Vec2(getX() / scalar, getY() / scalar); }
bool Vec2::operator==(const Vec2& other) const { return getX() == other.getX() && getY() == other.getY(); }

float Vec2::getLength() const {
	return std::sqrt(getX() * getX() + getY() * getY());
}

float Vec2::getLengthSquared() const {
	return getX() * getX() + getY() * getY();
}

float Vec2::getDistance(const Vec2& other) const {
	float dx = other.getX() - getX();
	float dy = other.getY() - getY();
	return std::sqrt(dx * dx + dy * dy);
}

float Vec2::getDistanceSquared(const Vec2& other) const {
	float dx = other.getX() - getX();
	float dy = other.getY() - getY();
	return dx * dx + dy * dy;
}

bool Vec2::isNearlyZero(float epsilon) const {
	return getLengthSquared() <= (epsilon * epsilon);
}

float Vec2::dot(const Vec2& other) const {
	return getX() * other.getX() + getY() * other.getY();
}

float Vec2::cross(const Vec2& other) const {
	return getX() * other.getY() - getY() * other.getX();
}

float Vec2::angle(const Vec2& other) const {
	float d = dot(other);
	float l = std::sqrt(getLengthSquared() * other.getLengthSquared());
	return (float)(std::acos(d / l) * 180.0/PI);
}

Vec2 Vec2::normalize() const {
	float len = getLength();
	return (len > 0.0f) ? Vec2(getX() / len, getY() / len) : Vec2();
}

Vec2 Vec2::normalizeRange(float min, float max) const {
	float len = getLength();
	if (len == 0.0f) return Vec2();
	float clamped = std::clamp(len, min, max);
	return Vec2(getX() / len * clamped, getY() / len * clamped);
}

Vec2 Vec2::clamp(const Vec2& min, const Vec2& max) const {
	return Vec2(std::clamp(getX(), min.getX(), max.getX()), std::clamp(getY(), min.getY(), max.getY()));
}

void Object::Vec2Bind::bind(Application* a) {
	sol::state_view view(a->GetLuaState());
	sol::usertype<Vec2> obj = view.new_usertype<Vec2>(
		"Vec2",
		sol::constructors<Vec2(), Vec2(float), Vec2(float, float)>(),
		sol::meta_function::addition, &Vec2::operator+,
		sol::meta_function::subtraction, &Vec2::operator-,
		sol::meta_function::multiplication, &Vec2::operator*,
		sol::meta_function::division, &Vec2::operator/,
		sol::meta_function::equal_to, &Vec2::operator==,

		"x", sol::property(&Vec2::getX, &Vec2::setX),
		"y", sol::property(&Vec2::getY, &Vec2::setY)
	);

	obj[sol::meta_function::to_string] = [](const Vec2& v) {
		return "(" + std::to_string(v.getX()) + ", " + std::to_string(v.getY()) + ")";
		};

	// Object Vec2

	// Field number x
	// Field number y

	// Constructor
	// Constructor number x, number y
	// Constructor number all

	// Returns the length of the vector.
	// Returns number
	obj.set_function("length", &Vec2::getLength);

	// Returns the length of the vector save the square root operation.
	// Returns number
	obj.set_function("lengthSqr", &Vec2::getLengthSquared);

	// Returns the distance between two vectors.
	// Params Vec2 other
	// Returns number
	obj.set_function("distance", &Vec2::getDistance);

	// Returns the distance between two vectors squared.
	// Params Vec2 other
	// Returns number
	obj.set_function("distanceSqr", &Vec2::getDistanceSquared);

	// Returns true if the vector is effectively zero.
	// Params number? epsilon
	// Returns boolean
	obj.set_function("isNearlyZero", &Vec2::isNearlyZero);

	// Measures alignment of two vectors; >0 - same direction, 0 - perpendicular, <0 - opposite.
	// Params Vec2 other
	// Returns number
	obj.set_function("dot", &Vec2::dot);

	// Measures signed scalar area, indicating clockwise versus counter-clockwise orientation.
	// Params Vec2 other
	// Returns number
	obj.set_function("cross", &Vec2::cross);

	// Measures the angle between vectors in degrees
	// Params Vec2 other
	// Returns number
	obj.set_function("angle", &Vec2::angle);

	// Returns a normalized unit vector.
	// Returns Vec2
	obj.set_function("normalize", &Vec2::normalize);

	// Returns a normalied vector scaled to clamp between numbers min and max.
	// Params number min, number max
	// Returns Vec2
	obj.set_function("normalizeRng", &Vec2::normalizeRange);

	// Returns a clamped vector to vectors min and max.
	// Params Vec2 min, Vec2 max
	// Returns Vec2
	obj.set_function("clamp", &Vec2::clamp);

	// End Object
}
