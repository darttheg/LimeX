#pragma once

#include <sol/forward.hpp>
#include <string>

class DebugConsole;
class Application;
class Renderer;

class Vec2;

namespace Module {
	namespace Lime {
		void bind(Application* app);

		namespace Bind {
			void SetDebugConfig(bool on, bool write = false);
			void Log(std::string msg, int color = 0);
			void SetEndOnError(bool v);
			void Close();
			bool SetInitConfig(int driverType);
			bool SetInitConfig(int driverType, const Vec2& windowSize);
			bool SetInitConfig(int driverType, const Vec2& windowSize, const Vec2& renderSize);
			int GetElapsedTime();
			std::string GetVersion();
			int GetFrameRate();
			void SetFrameRate(int fps);
			bool GetVSync();
			void SetVSync(bool on);
			int GetMemoryUsage();
		}
	}
}