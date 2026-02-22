#include "Modules/ModuleWindow.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include "Window.h"

#include "Objects/Event.h"
#include "Objects/Texture.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;
static Window* w;

void Module::Window::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();
	w = app->GetWindow();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Window
	sol::table module = lua["Lime"]["Window"].get_or_create<sol::table>();

	// Sets the window's title to `title`.
	// Params string title
	// Returns void
	module.set_function("setTitle", &Module::Window::Bind::SetTitle);

	// Toggles fullscreen mode.
	// Params boolean fullscreen
	// Returns void
	module.set_function("setFullscreen", &Module::Window::Bind::SetFullscreen);

	// Returns the window's position.
	// Returns Vec2
	module.set_function("getPosition", &Module::Window::Bind::GetPosition);

	// Sets the window's position to `pos`.
	// Params Vec2 pos
	// Returns void
	module.set_function("setPosition", &Module::Window::Bind::SetPosition);

	// Returns the size of the window.
	// Returns Vec2
	module.set_function("getSize", &Module::Window::Bind::GetSize);

	// Sets the window's size to `size`.
	// Params Vec2 size
	// Returns void
	module.set_function("setSize", &Module::Window::Bind::SetSize);

	// Returns the size of the monitor the window is running on.
	// Returns Vec2
	module.set_function("getMonitorSize", &Module::Window::Bind::GetMonitorSize);

	// Returns true if the window is focused.
	// Returns boolean
	module.set_function("isFocused", &Module::Window::Bind::IsFocused);

	// Allows the window to be resizable or locked to its intended size.
	// Params boolean allow
	// Returns void
	module.set_function("setResizable", &Module::Window::Bind::SetResizable);

	// Locks the aspect ratio of the raw window size regardless of resizing/maximizing.
	// Params boolean locked
	// Returns void
	module.set_function("setLockAspectRatio", &Module::Window::Bind::SetMaintainAR);

	// Field Event onResize, Event called by Lime once the window is resized in any way.
	w->WindowResize = std::make_shared<Event>();
	module["onResize"] = w->WindowResize;

	// End Module
}

// Functions

void Module::Window::Bind::SetTitle(std::string title) {
	w->setTitle(title);
}

void Module::Window::Bind::SetFullscreen(bool v) {
	w->doFullscreen(v);
}

Vec2 Module::Window::Bind::GetPosition() {
	return w->getPosition();
}

void Module::Window::Bind::SetPosition(const Vec2& pos) {
	w->setPosition(pos);
}

Vec2 Module::Window::Bind::GetSize() {
	return w->getSize();
}

void Module::Window::Bind::SetSize(const Vec2& size) {
	w->setSize(size);
}

Vec2 Module::Window::Bind::GetMonitorSize() {
	return w->getMonitorSize();
}

bool Module::Window::Bind::IsFocused() {
	return w->isFocused();
}

void Module::Window::Bind::SetResizable(bool on) {
	w->setResizable(on);
}

void Module::Window::Bind::SetMaintainAR(bool on) {
	w->keepAspectRatio(on);
}
