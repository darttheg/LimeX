#pragma once

#include <sol/forward.hpp>
#include <string>

class Application;

class Vec2;

namespace Module {
	namespace Input {
		void bind(Application* app);

		namespace Bind {
			bool isKeyDown(int key);
			bool isMouseButtonDown(int button);
			Vec2 getMouseDelta();
			Vec2 getMousePosition();
			void setMousePosition(const Vec2& pos);
			void setMouseVisible(bool vis);
			bool isButtonDown(int id, int btn);
			float getControllerAxis(int id, int axis);
			bool isControllerConnected(int id);
		}
	}
}