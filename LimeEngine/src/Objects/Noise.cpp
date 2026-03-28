#include "Objects/Noise.h"
#include <sol/sol.hpp>

Noise::Noise() : Noise(0) {
}

Noise::Noise(int seed) : Noise(seed, 8) {
}

Noise::Noise(int seed, int octaves) {
	setNoiseSeed(seed);
	setNoiseOctaves(octaves);
}

void Noise::setNoiseSeed(int seed) {
	p.setSeed(seed);
}

int Noise::getNoiseSeed() const {
	return p.seed;
}

void Noise::setNoiseOctaves(int oc) {
	p.setOctaves(oc);
}

int Noise::getNoiseOctaves() const {
	return p.octaves;
}

float Noise::getNoiseValue1D(float x) {
	return p.getValueAt(0, x);
}

float Noise::getNoiseValue2D(float x, float y) {
	return p.getValueAt(1, x, y);
}

float Noise::getNoiseValue3D(float x, float y, float z) {
	return p.getValueAt(2, x, y, z);
}

void Object::NoiseBind::bind(lua_State* ls) {
	sol::state_view view(ls);

	// Object Noise, An object that holds gradient noise. Useful for random terrain, clouds, and more.
	// Constructor
	// Constructor number seed
	// Constructor number seed, number octaves

	sol::usertype<Noise> obj = view.new_usertype<Noise>(
		"Noise",
		sol::constructors<Noise(), Noise(int seed), Noise(int seed, int octaves)>(),
		sol::meta_function::type, [](const Noise&) { return "Noise"; },

		// Field number seed, Sets the seed of this `Noise`.
		"seed", sol::property(&Noise::getNoiseSeed, &Noise::setNoiseSeed),

		// Field number octaves, Sets the number of octaves in this `Noise`.
		"octaves", sol::property(&Noise::getNoiseOctaves, &Noise::setNoiseOctaves)
	);

	obj[sol::meta_function::to_string] = [](const Noise& v) {
		return v.getNoiseSeed();
	};

	// Returns the value at `x`, `y`, `z` in this `Noise` object.
	// Params number x
	// Params number x, number y
	// Params number x, number y, number z
	// Returns number
	obj.set_function("get",
		sol::overload(
			sol::resolve<float(float x)>(&Noise::getNoiseValue1D),
			sol::resolve<float(float x, float y)>(&Noise::getNoiseValue2D),
			sol::resolve<float(float x, float y, float z)>(&Noise::getNoiseValue3D)
		));

	// End Object
}