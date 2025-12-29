#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

namespace Module {
	namespace Lime {
		void init(DebugConsole* deb, Application* app, Renderer* rend);
		void bind(sol::state& lua);

		void Log(std::string msg, int color = 0);
		void DisplayMessage(std::string msg, std::string title = "Lime Error!", int icon = 1);
		void SetEndOnError(bool v);
		void Close();
		std::string GetVersion();
		int GetTime();
		float GetDeltaTime();
		int GetFrameRate();
		void SetFrameLimit(int limit);
		int GetMemoryUsage();
		void SetVSync(bool v);
		void SetShowDebugger(bool v);
		void SetWriteDebugOutput(bool v);
	}
}