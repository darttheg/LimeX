#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

namespace Module {
	namespace Lime {
		void bind(Application* app);

		namespace Bind {
			void Log(std::string msg, int color = 0);
			void SetEndOnError(bool v);
			void Close();
			std::string GetVersion();
		}
	}
}