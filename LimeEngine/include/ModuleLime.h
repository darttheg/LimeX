#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;

namespace Module {
	namespace Lime {
		void init(DebugConsole* deb, Application* app);
		void bind(sol::state& lua);

		void Log(std::string msg, int color = 0);
		void DisplayMessage(std::string msg, std::string title = "Lime Error!", int icon = 1);
		void SetEndOnError(bool v);
		void Close();
		std::string GetVersion();
		int GetTime();
		float GetDeltaTime();
		int GetFrameRate();
		void SetFrameRate(int limit);
		std::string GetPlatform();
		int GetMemoryUsage();
	}
}