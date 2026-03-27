#include "Modules/ModuleHelp.h"

#include "Application.h"

#include <filesystem>
#include <fstream>
#include <sol/sol.hpp>
#include "External/PerlinHeader.h"

static Application* a;
static sol::state* l;
static PerlinObject p;

void Module::Help::bind(Application* app) {
	a = app;
	l = &app->GetLuaState();
	sol::state& lua = app->GetLuaState();

	// Module math.noise
	sol::table module = lua["math"]["noise"].get_or_create<sol::table>();

	// Sets the seed of the noise map.
	// Params number seed
	// Returns void
	module.set_function("setSeed", &Module::Help::Bind::setNoiseSeed);

	// Returns the seed of the noise map.
	// Returns number
	module.set_function("getSeed", &Module::Help::Bind::getNoiseSeed);

	// Sets the number of octaves in the noise map.
	// Params number octaves
	// Returns void
	module.set_function("setOctaves", &Module::Help::Bind::setNoiseOctaves);

	// Returns the number of octaves in the noise map.
	// Returns number
	module.set_function("getOctaves", &Module::Help::Bind::getNoiseOctaves);

	// Returns the value found in the noise map.
	// Params number x
	// Params number x, number y
	// Params number x, number y, number z
	// Returns number
	module.set_function("getValue",
		sol::overload(
			sol::resolve<float(float x)>(&Module::Help::Bind::getNoiseValue1D),
			sol::resolve<float(float x, float y)>(&Module::Help::Bind::getNoiseValue2D),
			sol::resolve<float(float x, float y, float z)>(&Module::Help::Bind::getNoiseValue3D)
		));

	// End Module
}

// Functions

void Module::Help::Bind::setNoiseSeed(int seed) {
	p.setSeed(seed);
}

int Module::Help::Bind::getNoiseSeed() {
	return p.seed;
}

void Module::Help::Bind::setNoiseOctaves(int oc) {
	p.setOctaves(oc);
}

int Module::Help::Bind::getNoiseOctaves() {
	return p.octaves;
}

float Module::Help::Bind::getNoiseValue1D(float x) {
	return p.getValueAt(0, x);
}

float Module::Help::Bind::getNoiseValue2D(float x, float y) {
	return p.getValueAt(1, x, y);
}

float Module::Help::Bind::getNoiseValue3D(float x, float y, float z) {
	return p.getValueAt(2, x, y, z);
}
