#include "Objects/Vec3.h"
#include "Application.h"

#define PI 3.14159265358979323846

Vec3::Vec3() : x(0), y(0) {}
Vec3::Vec3(float x) : x(x), y(x), z(x) {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3& other) const { return Vec3(getX() + other.getX(), getY() + other.getY(), getZ() + other.getZ()); }
Vec3 Vec3::operator-(const Vec3& other) const { return Vec3(getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ()); }
Vec3 Vec3::operator*(float scalar) const { return Vec3(getX() * scalar, getY() * scalar, getZ() * scalar); }
Vec3 Vec3::operator/(float scalar) const { return Vec3(getX() / scalar, getY() / scalar, getZ() / scalar); }
bool Vec3::operator==(const Vec3& other) const { return getX() == other.getX() && getY() == other.getY() && getZ() == other.getZ(); }

void Object::Vec3Bind::bind(Application* a) {
	sol::state_view view(a->GetLuaState());
	sol::usertype<Vec3> obj = view.new_usertype<Vec3>(
		"Vec3",
		sol::constructors<Vec3(), Vec3(float), Vec3(float, float, float)>(),
		sol::meta_function::addition, &Vec3::operator+,
		sol::meta_function::subtraction, &Vec3::operator-,
		sol::meta_function::multiplication, &Vec3::operator*,
		sol::meta_function::division, &Vec3::operator/,
		sol::meta_function::equal_to, &Vec3::operator==,

		"x", sol::property(&Vec3::getX, &Vec3::setX),
		"y", sol::property(&Vec3::getY, &Vec3::setY),
		"z", sol::property(&Vec3::getZ, &Vec3::setZ)
	);

	// Object Vec3

	// Field number x
	// Field number y
	// Field number z

	// Constructor
	// Constructor number x, number y, number z
	// Constructor number all

	// End Object
}