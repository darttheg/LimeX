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

	// Object Vec2

	// Field number x
	// Field number y

	// Constructor
	// Constructor number x, number y
	// Constructor number all

	// End Object
}
