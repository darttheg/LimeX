#include "Objects/Vec4.h"
#include "Application.h"

#include <iomanip>

#define PI 3.14159265358979323846

Vec4::Vec4() : x(0), y(0) {}
Vec4::Vec4(float x) : x(x), y(x), z(x), w(x) {}
Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Vec4 Vec4::operator+(const Vec4& other) const { return Vec4(getX() + other.getX(), getY() + other.getY(), getZ() + other.getZ(), getW() + other.getW()); }
Vec4 Vec4::operator-(const Vec4& other) const { return Vec4(getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ(), getW() - other.getW()); }
Vec4 Vec4::operator*(float scalar) const { return Vec4(getX() * scalar, getY() * scalar, getZ() * scalar, getW() * scalar); }
Vec4 Vec4::operator/(float scalar) const { return Vec4(getX() / scalar, getY() / scalar, getZ() / scalar, getW() / scalar); }
bool Vec4::operator==(const Vec4& other) const { return getX() == other.getX() && getY() == other.getY() && getZ() == other.getZ() && getW() == other.getW(); }

std::string Vec4::getHex() {
	auto clamp = [](int v) {
		return std::clamp(v, 0, 255);
	};

	std::ostringstream out;
	out << '#'
		<< std::uppercase << std::hex << std::setfill('0')
		<< std::setw(2) << clamp(getX())
		<< std::setw(2) << clamp(getY())
		<< std::setw(2) << clamp(getZ())
		<< std::setw(2) << clamp(getW());

	return out.str();
}

void Object::Vec4Bind::bind(Application* a) {
	sol::state_view view(a->GetLuaState());
	sol::usertype<Vec4> obj = view.new_usertype<Vec4>(
		"Vec4",
		sol::constructors<Vec4(), Vec4(float), Vec4(float, float, float, float)>(),
		sol::meta_function::type, [](const Vec4&) { return "Vec4"; },

		sol::meta_function::addition, &Vec4::operator+,
		sol::meta_function::subtraction, &Vec4::operator-,
		sol::meta_function::multiplication, &Vec4::operator*,
		sol::meta_function::division, &Vec4::operator/,
		sol::meta_function::equal_to, &Vec4::operator==,

		"x", sol::property(&Vec4::getX, &Vec4::setX),
		"y", sol::property(&Vec4::getY, &Vec4::setY),
		"z", sol::property(&Vec4::getZ, &Vec4::setZ),
		"w", sol::property(&Vec4::getW, &Vec4::setW)
	);

	obj[sol::meta_function::to_string] = [](const Vec4& v) {
		return "(" + std::to_string(v.getX()) + ", " + std::to_string(v.getY()) + std::to_string(v.getZ()) + std::to_string(v.getW()) + ")";
		};

	// Object Vec4, A four-dimensional vector object.

	// Field number x
	// Field number y
	// Field number z
	// Field number w

	// Constructor
	// Constructor number x, number y, number z, number w
	// Constructor number all

	// Returns the HEX code for this object. This is useful for converting RGBA to HEX color.
	// Returns string
	obj.set_function("getHEX", &Vec4::getHex);

	// End Object
}