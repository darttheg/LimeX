#pragma once

class Vec3;
#include "sol/forward.hpp"

class HitResult {
public:
    HitResult() = default;

	Vec3 startPos;
	Vec3 endPos;
	Vec3 normal;
	int objID = 0;
	int matID = 0;
	bool hit = false;
	sol::table attr;
};