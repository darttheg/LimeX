#include "Modules/ModuleLime.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"

#include "Objects/Event.h"
#include "Objects/Vec2.h"

#include <sol/sol.hpp>

static void bindEnums(sol::state& lua, sol::table module) {
#define LIME_MODULE(name)
#define LIME_ENUM(name, body) \
do { \
	sol::table foo = lua.create_table(); \
	body \
	module[#name] = foo; \
} while (0)

#define LIME_ENUM_VALUE(key, val) \
	foo[#key] = val;

#define LIME_DOC(text)
#define LIME_FUNC(returnType, name, ...)

#include "Enums.api.inl"

#undef LIME_FUNC
#undef LIME_DOC
#undef LIME_ENUM_VALUE
#undef LIME_ENUM
#undef LIME_MODULE
}

static DebugConsole* d;
static Application* a;
static Renderer* r;

void Module::Lime::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();
	sol::state& lua = app->GetLuaState();

	// Module Lime
	sol::table module = lua["Lime"].get_or_create<sol::table>();
	sol::table enums = lua["Lime"]["Enum"].get_or_create<sol::table>();
	bindEnums(lua, enums);

	// Prints a message to console.
	// Params any msg, Lime.Enum.PrintColor? color
	// Returns void
	module.set_function("log", [](sol::this_state ts, sol::object obj) {
		sol::state_view L(ts);
		sol::function tostringfn = L["tostring"];
		std::string s = tostringfn(obj);

		Module::Lime::Bind::Log(s);
	});

	// If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
	// Params boolean doEnd
	module.set_function("setEndOnError", &Module::Lime::Bind::SetEndOnError);

	// Closes the Lime application.
	module.set_function("close", &Module::Lime::Bind::Close);

	// Returns the Lime version running.
	// Returns string
	module.set_function("getVersion", &Module::Lime::Bind::GetVersion);

	// IMPORTANT: This function should always be run prior to window creation (pre-`Lime.onUpdate` Event) as only here can the driver type be changed. This function sets initial parameters for the Lime application.
	// Params Lime.Enum.DriverType driver, Vec2? windowSize, Vec2? renderSize
	// Returns boolean
	module.set_function("setInitConfig", &Module::Lime::Bind::SetInitConfig);

	// If true, Lime will not render the scene at the end of each `Lime.onUpdate` cycle. Instead, use `Lime.Scene.render` for 3D scene rendering and `Lime.GUI.render` for GUI rendering within the `Lime.onUpdate` Event.
	// Params boolean isManual
	// Returns void
	module.set_function("setManualRendering", &Module::Lime::Bind::SetManualRendering);

	// Returns the elapsed time the application has been running in milliseconds.
	// Returns number
	module.set_function("getElapsedTime", &Module::Lime::Bind::GetElapsedTime);

	// Returns the frame rate.
	// Returns number
	module.set_function("getFrameRate", &Module::Lime::Bind::GetFrameRate);

	// Sets the target frame rate.
	// Params number target
	// Returns void
	module.set_function("setFrameRate", &Module::Lime::Bind::SetFrameRate);

	// Sets vertical syncronization, matching the frame rate to the primary monitor's refresh rate.
	// Params boolean vSyncOn
	// Returns void
	module.set_function("setVSync", &Module::Lime::Bind::SetVSync);

	// Returns true if vertical syncronization is on.
	// Returns boolean
	module.set_function("getVSync", &Module::Lime::Bind::GetVSync);

	a->LimeInit = std::make_shared<Event>();
	a->LimeStart = std::make_shared<Event>();
	a->LimeUpdate = std::make_shared<Event>(); // Call with dt
	a->LimeEnd = std::make_shared<Event>(); // Call with bool isError?

	// Field Event onInit, Event called by Lime prior to initializing the window.
	// Field Event onStart, Event called by Lime following window creation and rendering services are available.
	// Field Event onUpdate, Event called by Lime every rendering frame. This Event is run with a number delta time argument.
	// Field Event onClose, Event called by Lime once the application closes in any way.
	module["onInit"] = a->LimeInit;
	module["onStart"] = a->LimeStart;
	module["onUpdate"] = a->LimeUpdate;
	module["onClose"] = a->LimeEnd;

	// End Module
}

// Functions

void Module::Lime::Bind::Log(std::string msg, int color) {
	d->Log(msg.c_str(), (MESSAGE_TYPE)color);
} 

void Module::Lime::Bind::SetEndOnError(bool v) {
	d->SetEndOnError(v);
}

void Module::Lime::Bind::Close() {
	a->EndApp();
}

std::string Module::Lime::Bind::GetVersion() {
	return LIME_VERSION;
}

int Module::Lime::Bind::GetFrameRate() {
	return a->getFrameRate();
}

void Module::Lime::Bind::SetFrameRate(int fps) {
	a->setTargetFrameRate(fps);
}

bool Module::Lime::Bind::GetVSync() {
	return a->getVSync();
}

void Module::Lime::Bind::SetVSync(bool on) {
	a->setVSync(on);
}

bool Module::Lime::Bind::SetInitConfig(int driverType, const Vec2& windowSize, const Vec2& renderSize) {
	if (a->IsRunning()) {
		d->Warn("Lime.setInitConfig was called but the window has already been created.");
		return false;
	}

	WindowConfig cfg = a->GetConfig();

	cfg.driverType = driverType;
	cfg.windowSize = std::vector<int>{ (int)windowSize.getX(), (int)windowSize.getY() };
	cfg.renderSize = std::vector<int>{ (int)renderSize.getX(), (int)renderSize.getY() };

	a->SetConfig(cfg);
	return true;
}

void Module::Lime::Bind::SetManualRendering(bool on) {
	r->setManualRendering(on);
}

int Module::Lime::Bind::GetElapsedTime() {
	return r->getElapsedTime();
}
