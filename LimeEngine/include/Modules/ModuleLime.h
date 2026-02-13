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
			void Log(std::string msg, int color = 0);
			void SetEndOnError(bool v);
			void Close();
			bool SetInitConfig(int driverType, const Vec2& windowSize, const Vec2& renderSize);
			void SetManualRendering(bool on = true);
			int GetElapsedTime();
			std::string GetVersion();
		}
	}
}