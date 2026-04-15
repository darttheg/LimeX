#pragma once

#include <sol/forward.hpp>
#include <string>

class Application;

namespace Module {
	namespace Lua {
		void bind(Application* app);
		void bindMath(sol::state& l);

		namespace Math {
			namespace Tween {
				float damp(float old, float target, float factor, float dt);
				float lerp(float old, float target, float time);
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
			}
		}
	}
}