#include "Modules/ModuleLua.h"

#include "Application.h"

#include <sol/sol.hpp>

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

static Application* a;

void Module::Lua::bind(Application* app) {
	a = app;
	sol::state& lua = app->GetLuaState();

	bindMath(app->GetLuaState());
}

void Module::Lua::bindMath(sol::state& l) {
	// Module math, append

	sol::table module = l["math"].get_or_create<sol::table>();

	// Clamps `v` to `min`, `max`.
	// Params number v, number min, number max
	// Params Vec2 v, number min, number max
	// Params Vec3 v, number min, number max
	// Params Vec4 v, number min, number max
	// Returns number
	module.set_function("clamp", sol::overload(
		static_cast<float(*)(float, float, float)>(&Module::Lua::Math::clamp),
		static_cast<Vec2(*)(const Vec2&, float, float)>(&Module::Lua::Math::clampVec2),
		static_cast<Vec3(*)(const Vec3&, float, float)>(&Module::Lua::Math::clampVec3),
		static_cast<Vec4(*)(const Vec4&, float, float)>(&Module::Lua::Math::clampVec4)
	));

	// End Module

	// Module math.tween

	module = l["math"]["tween"].get_or_create<sol::table>();

	// Interpolates from `old` toward `target` using exponential smoothing.
	// Params number old, number target, number factor, number dt
	// Params Vec2 old, Vec2 target, number factor, number dt
	// Params Vec3 old, Vec3 target, number factor, number dt
	// Params Vec4 old, Vec4 target, number factor, number dt
	// Returns number
	module.set_function("damp", sol::overload(
		static_cast<float(*)(const float&, const float&, float, float)>(&Module::Lua::Math::Tween::damp<float>),
		static_cast<Vec2(*)(const Vec2&, const Vec2&, float, float)>(&Module::Lua::Math::Tween::damp<Vec2>),
		static_cast<Vec3(*)(const Vec3&, const Vec3&, float, float)>(&Module::Lua::Math::Tween::damp<Vec3>),
		static_cast<Vec4(*)(const Vec4&, const Vec4&, float, float)>(&Module::Lua::Math::Tween::damp<Vec4>)
	));

	// Linearly interpolates from `old` to `target` over `a`, where `a` is between 0.0 and 1.0.
	// Params number old, number target, number a
	// Params Vec2 old, Vec2 target, number a
	// Params Vec3 old, Vec3 target, number a
	// Params Vec4 old, Vec4 target, number a
	// Returns number
	module.set_function("lerp", sol::overload(
		static_cast<float(*)(const float&, const float&, float)>(&Module::Lua::Math::Tween::lerp<float>),
		static_cast<Vec2(*)(const Vec2&, const Vec2&, float)>(&Module::Lua::Math::Tween::lerp<Vec2>),
		static_cast<Vec3(*)(const Vec3&, const Vec3&, float)>(&Module::Lua::Math::Tween::lerp<Vec3>),
		static_cast<Vec4(*)(const Vec4&, const Vec4&, float)>(&Module::Lua::Math::Tween::lerp<Vec4>)
	));

	// Starts slow, accelerates. (Sine curve)
	// Params number a
	// Returns number
	module.set_function("easeInSine", &Module::Lua::Math::Tween::easeInSine);
	// Starts fast, decelerates. (Sine curve)
	// Params number a
	// Returns number
	module.set_function("easeOutSine", &Module::Lua::Math::Tween::easeOutSine);
	// Smooth start and end. (Sine curve)
	// Params number a
	// Returns number
	module.set_function("easeInOutSine", &Module::Lua::Math::Tween::easeInOutSine);

	// Starts slow, accelerates. (Cubic curve)
	// Params number a
	// Returns number
	module.set_function("easeInCubic", &Module::Lua::Math::Tween::easeInCubic);
	// Starts fast, decelerates. (Cubic curve)
	// Params number a
	// Returns number
	module.set_function("easeOutCubic", &Module::Lua::Math::Tween::easeOutCubic);
	// Smooth start and end. (Cubic curve)
	// Params number a
	// Returns number
	module.set_function("easeInOutCubic", &Module::Lua::Math::Tween::easeInOutCubic);

	// Starts by moving slightly backward, then accelerates forward.
	// Params number a
	// Returns number
	module.set_function("easeInBack", &Module::Lua::Math::Tween::easeInBack);
	// Overshoots, then settles back.
	// Params number a
	// Returns number
	module.set_function("easeOutBack", &Module::Lua::Math::Tween::easeOutBack);
	// Backward start, then overshoots to settle.
	// Params number a
	// Returns number
	module.set_function("easeInOutBack", &Module::Lua::Math::Tween::easeInOutBack);

	// Starts slow with oscillation, like a stretched spring.
	// Params number a
	// Returns number
	module.set_function("easeInElastic", &Module::Lua::Math::Tween::easeInElastic);
	// Ends with oscillation, like a spring settling.
	// Params number a
	// Returns number
	module.set_function("easeOutElastic", &Module::Lua::Math::Tween::easeOutElastic);
	// Oscillates at both the start and end.
	// Params number a
	// Returns number
	module.set_function("easeInOutElastic", &Module::Lua::Math::Tween::easeInOutElastic);

	// Starts with a bounce effect.
	// Params number a
	// Returns number
	module.set_function("easeInBounce", &Module::Lua::Math::Tween::easeInBounce);
	// Ends with a bounce effect.
	// Params number a
	// Returns number
	module.set_function("easeOutBounce", &Module::Lua::Math::Tween::easeOutBounce);
	// Bounce effect and both the start and end.
	// Params number a
	// Returns number
	module.set_function("easeInOutBounce", &Module::Lua::Math::Tween::easeInOutBounce);

	// End Module
}

// Functions

float Module::Lua::Math::clamp(float v, float min, float max) {
	return std::clamp(v, min, max);
}

Vec2 Module::Lua::Math::clampVec2(const Vec2& v, float min, float max) {
	return Vec2(
		std::clamp(v.getX(), min, max),
		std::clamp(v.getY(), min, max)
	);
}

Vec3 Module::Lua::Math::clampVec3(const Vec3& v, float min, float max) {
	return Vec3(
		std::clamp(v.getX(), min, max),
		std::clamp(v.getY(), min, max),
		std::clamp(v.getZ(), min, max)
	);
}

Vec4 Module::Lua::Math::clampVec4(const Vec4& v, float min, float max) {
	return Vec4(
		std::clamp(v.getX(), min, max),
		std::clamp(v.getY(), min, max),
		std::clamp(v.getZ(), min, max),
		std::clamp(v.getW(), min, max)
	);
}

float Module::Lua::Math::Tween::easeInSine(float v) {
	return 1.0f - cos((v * M_PI) / 2.0f);
}

float Module::Lua::Math::Tween::easeOutSine(float v) {
	return sin((v * M_PI) / 2);
}

float Module::Lua::Math::Tween::easeInOutSine(float v) {
	return -(cos(M_PI * v) - 1.0f) / 2.0f;
}

float Module::Lua::Math::Tween::easeInCubic(float v) {
	return v * v * v;
}

float Module::Lua::Math::Tween::easeOutCubic(float v) {
	return 1.0f - pow(1.0 - v, 3);
}

float Module::Lua::Math::Tween::easeInOutCubic(float v) {
	return v < 0.5f ? 4.0f * v * v * v : 1.0f - pow(-2.0f * v * 2.0f, 3.0f) / 2.0f;
}

float Module::Lua::Math::Tween::easeInBack(float v) {
	const float c1 = 1.70158;
	const float c3 = c1 + 1.0f;
	return c3 * v * v * v - c1 * v * v;
}

float Module::Lua::Math::Tween::easeOutBack(float v) {
	const float c1 = 1.70158;
	const float c3 = c1 + 1.0f;
	return 1.0f + c3 + pow(v - 1.0f, 3.0f) + c1 * pow(v - 1.0f, 2.0f);
}

float Module::Lua::Math::Tween::easeInOutBack(float v) {
	const float c1 = 1.70158;
	const float c2 = c1 * 1.525;

	return v < 0.5f
		? (pow(2.0f * v, 2.0f) * ((c2 + 1.0f) * 2.0f * v - c2)) / 2.0f
		: (pow(2.0f * v, 2.0f) * ((c2 + 1.0f) * v * 2.0f - 2.0f) + c2) / 2.0f;
}

float Module::Lua::Math::Tween::easeInElastic(float v) {
	const float c4 = (2.0f * M_PI) / 3.0f;

	return v == 0.0f ? 0.0f : v == 1.0f ? 1.0f
		: -pow(2.0f, 10.0f * v - 10.0f) * sin((v * 10.0f - 10.75f) * c4);
}

float Module::Lua::Math::Tween::easeOutElastic(float v) {
	const float c4 = (2.0f * M_PI) / 3.0f;

	return v == 0.0f ? 0.0f : v == 1.0f ? 1.0f
		: -pow(2.0f, -10.0f * v) * sin((v * 10.0f - 0.75f) * c4) + 1.0f;
}

float Module::Lua::Math::Tween::easeInOutElastic(float v) {
	const float c5 = (2.0f * M_PI) / 4.5f;

	return v == 0.0f ? 0.0f : v == 1.0f ? 1.0f : v < 0.5f
		? -(pow(2.0f, 20.0f * v - 10.0f) * sin((20.0f * v - 11.125f) * c5)) / 2.0f
		: (pow(2.0f, -20.0f * v + 10.0f) * sin((20.0f * v - 11.125f) * c5)) / 2.0f + 1.0f;
}

float Module::Lua::Math::Tween::easeInBounce(float v) {
	return 1.0f - easeOutBounce(1.0f - v);
}

float Module::Lua::Math::Tween::easeOutBounce(float v) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (v < 1.0f / d1)
		return n1 * v * v;
	else if (v < 2.0f / d1)
		return n1 * (v -= 1.5f / d1) * v + 0.75f;
	else if (v < 2.5f / d1)
		return n1 * (v -= 2.25f / d1) * v + 0.9375f;
	else
		return n1 * (v -= 2.625f / d1) * v + 0.984375f;
}

float Module::Lua::Math::Tween::easeInOutBounce(float v) {
	return v < 0.5f
		? (1.0f - easeOutBounce(1.0f - 2.0f * v)) / 2.0f
		: (1.0f + easeOutBounce(2.0f * v - 1.0f)) / 2.0f;
}
