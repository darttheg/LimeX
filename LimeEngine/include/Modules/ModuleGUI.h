#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

class Vec2;
class Vec3;
class Vec4;
class Image;
//class Mesh;

namespace Module {
	namespace GUI {
		void bind(Application* app);

		namespace Bind {
			bool render();
			std::string embedFont(const std::string& path);
			void setDefaultFont(const std::string& name);
			bool isFontEmbeded(const std::string& name);
			void setQuality(int q);
		}
	}
}