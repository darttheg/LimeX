#include "Objects/Vec4.h"
#include <sol/sol.hpp>

#include <iomanip>

#define PI 3.14159265358979323846

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

static int hexNibble(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
	return -1;
}

static bool hexByte(char hi, char lo, int& out) {
	int h = hexNibble(hi);
	int l = hexNibble(lo);
	if (h < 0 || l < 0) return false;
	out = (h << 4) | l;
	return true;
}

static Vec4 fromHex(std::string_view hex) {
	while (!hex.empty() && std::isspace(static_cast<unsigned char>(hex.front()))) hex.remove_prefix(1);
	while (!hex.empty() && std::isspace(static_cast<unsigned char>(hex.back())))  hex.remove_suffix(1);

	if (!hex.empty() && hex.front() == '<') hex.remove_prefix(1);
	if (!hex.empty() && hex.back() == '>') hex.remove_suffix(1);
	if (!hex.empty() && hex.front() == '#') hex.remove_prefix(1);

	if (hex.size() != 6 && hex.size() != 8) return Vec4(0, 0, 0, 255);

	int r = 0, g = 0, b = 0, a = 255;

	if (!hexByte(hex[0], hex[1], r)) return Vec4(0, 0, 0, 255);
	if (!hexByte(hex[2], hex[3], g)) return Vec4(0, 0, 0, 255);
	if (!hexByte(hex[4], hex[5], b)) return Vec4(0, 0, 0, 255);

	if (hex.size() == 8)
		if (!hexByte(hex[6], hex[7], a)) return Vec4(0, 0, 0, 255);

	return Vec4(r, g, b, a);
}

Vec4::Vec4() : x(0), y(0), z(0), w(0) {}
Vec4::Vec4(float x) : x(x), y(x), z(x), w(x) {}
Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Vec4::Vec4(const std::string& hex) {
	Vec4 out = fromHex(hex);

	setX(out.getX());
	setY(out.getY());
	setZ(out.getZ());
	setW(out.getW());
}

Vec4 Vec4::operator+(const Vec4& other) const { return Vec4(getX() + other.getX(), getY() + other.getY(), getZ() + other.getZ(), getW() + other.getW()); }
Vec4 Vec4::operator-(const Vec4& other) const { return Vec4(getX() - other.getX(), getY() - other.getY(), getZ() - other.getZ(), getW() - other.getW()); }
Vec4 Vec4::operator*(float scalar) const { return Vec4(getX() * scalar, getY() * scalar, getZ() * scalar, getW() * scalar); }
Vec4 Vec4::operator/(float scalar) const { return Vec4(getX() / scalar, getY() / scalar, getZ() / scalar, getW() / scalar); }
bool Vec4::operator==(const Vec4& other) const { return getX() == other.getX() && getY() == other.getY() && getZ() == other.getZ() && getW() == other.getW(); }

void Object::Vec4Bind::bind(lua_State* ls) {
	sol::state_view view(ls);
	sol::usertype<Vec4> obj = view.new_usertype<Vec4>(
		"Vec4",
		sol::constructors<Vec4(), Vec4(float), Vec4(float, float, float, float), Vec4(const std::string&)>(),
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
		return "(" + std::to_string(v.getX()) + ", " + std::to_string(v.getY()) + ", " + std::to_string(v.getZ()) + ", " + std::to_string(v.getW()) + ")";
		};

	// Object Vec4, A four-dimensional vector object.

	// Field number x
	// Field number y
	// Field number z
	// Field number w

	// Constructor
	// Constructor number x, number y, number z, number w
	// Constructor number all
	// Constructor string HEX

	// Operation Vec4 Vec4 +
	// Operation Vec4 Vec4 -
	// Operation Vec4 number *
	// Operation Vec4 number /
	// Operation boolean Vec4 ==

	// Returns a copy of this vector.
	// Returns Vec4
	obj.set_function("copy", &Vec4::copy);

	// Returns the HEX code for this object. This is useful for converting RGBA to HEX color.
	// Returns string
	obj.set_function("getHEX", &Vec4::getHex);

	// End Object
}