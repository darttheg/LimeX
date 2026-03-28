#pragma once
#include <string>
#include <cmath>
#include <functional>
#include "External/PerlinHeader.h"

extern "C" { struct lua_State; }

class Noise {
public:
	Noise();
	Noise(int seed);
	Noise(int seed, int octaves);

	void setNoiseSeed(int seed);
	int getNoiseSeed() const;
	void setNoiseOctaves(int oc);
	int getNoiseOctaves() const;
	float getNoiseValue1D(float x);
	float getNoiseValue2D(float x, float y);
	float getNoiseValue3D(float x, float y, float z);
private:
    PerlinObject p;
};

namespace Object::NoiseBind {
    void bind(lua_State* ls);
}