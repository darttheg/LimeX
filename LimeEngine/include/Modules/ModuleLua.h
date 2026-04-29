#pragma once

#include <sol/forward.hpp>
#include <string>
#include <cmath>

class Application;
class Vec2;
class Vec3;
class Vec4;

namespace Module {
	namespace Lua {
		void bind(Application* app);
		void bindMath(sol::state& l);

		namespace Math {
			float clamp(float v, float min, float max);
			Vec2 clampVec2(const Vec2&, float min, float max);
			Vec3 clampVec3(const Vec3&, float min, float max);
			Vec4 clampVec4(const Vec4&, float min, float max);

			namespace Tween {
				template<typename T>
				T damp(const T& old, const T& target, float factor, float dt) {
					return old + (target - old) * (1.0f - std::exp(-factor * dt));
				}

				template<typename T>
				T lerp(const T& old, const T& target, float time) {
					return old + (target - old) * time;
				}

				float easeInSine(float v);
				float easeOutSine(float v);
				float easeInOutSine(float v);
				float easeInCubic(float v);
				float easeOutCubic(float v);
				float easeInOutCubic(float v);
				float easeInBack(float v);
				float easeOutBack(float v);
				float easeInOutBack(float v);
				float easeInElastic(float v);
				float easeOutElastic(float v);
				float easeInOutElastic(float v);
				float easeInBounce(float v);
				float easeOutBounce(float v);
				float easeInOutBounce(float v);
				float easeInExpo(float v);
				float easeOutExpo(float v);
				float easeInOutExpo(float v);
			}
		}
	}
}