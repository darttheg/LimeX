#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;
class Vec2;

#define LIME_VERSION "beta-0.21"

namespace Module {
	namespace Lime {
		void bind(Application* app);

		namespace Bind {
			void SetDebugConfig(bool on, bool write = false, bool suppressWarnings = false);
			void ClearDebug();
			void Log(std::string msg, int color = 0);
			void SetEndOnError(bool v);
			void Close();
			bool SetInitConfig(int driverType);
			bool SetInitConfig(int driverType, const Vec2& windowSize);
			bool SetInitConfig(int driverType, const Vec2& windowSize, const Vec2& renderSize);
			int GetElapsedTime();
			float GetElapsedTimeSeconds();
			std::string GetVersion();
			int GetFrameRate();
			void SetFrameRate(int fps);
			bool GetVSync();
			void SetVSync(bool on);
			int GetMemoryUsage();
			int ExecuteCommandLine(const std::string& cmd);
			sol::object GetCommandLineEntry(const std::string& key);
			void DisplayMessage(const std::string& title, const std::string message, int img = 0);
			bool AddArchive(const std::string& path);
		}
	}
}