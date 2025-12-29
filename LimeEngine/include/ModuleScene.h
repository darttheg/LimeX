#pragma once

#include <sol/forward.hpp>
#include <string>

class Renderer;
class DebugConsole;

namespace Module {
	namespace Scene {
		void init(Renderer* rend, DebugConsole* deb);
		void bind(sol::state& lua);
	}
}