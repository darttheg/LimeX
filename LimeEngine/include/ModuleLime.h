#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

namespace Lime {
	void bind(sol::state& lua, Application* app);

	namespace Bind {
		void Log(std::string msg, int color = 0);
		void SetEndOnError(bool v);
		void Close();
		std::string GetVersion();
	}
}