#pragma once

class Vec3;

class HitResult {
public:
    HitResult() = default;

	Vec3 startPos;
	Vec3 endPos;
	Vec3 normal;
	int objID = 0;
	int matID = 0;
	bool hit = false;
};