#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;
class Window;

class Vec2;
class Vec3;
class Vec4;
class Texture;

namespace Module {
	namespace Window {
		void bind(Application* app);

		namespace Bind {
			void SetTitle(std::string title);
			void SetFullscreen(bool v);
			Vec2 GetPosition();
			void SetPosition(const Vec2& pos);
			Vec2 GetSize();
			void SetSize(const Vec2& size);
			Vec2 GetMonitorSize();
			bool IsFocused();
			void SetResizable(bool on);
			void SetMaintainAR(bool on);
		}
	}
}