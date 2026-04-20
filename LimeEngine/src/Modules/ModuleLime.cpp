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
	module.set_function("log", [](sol::this_state ts, sol::object obj, sol::optional<int> color) {
		sol::state_view L(ts);
		sol::function tostringfn = L["tostring"];
		std::string s = tostringfn(obj);

		Module::Lime::Bind::Log(s, color.value_or(0));
	});

	// If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
	// Params boolean doEnd
	module.set_function("setEndOnError", &Module::Lime::Bind::SetEndOnError);

	// Closes the Lime application.
	module.set_function("close", &Module::Lime::Bind::Close);

	// Clears the debug console's lines, only visually.
	module.set_function("clearDebugConsole", &Module::Lime::Bind::ClearDebug);

	// Returns the Lime version running.
	// Returns string
	module.set_function("getVersion", &Module::Lime::Bind::GetVersion);

	// [-] This function sets initial parameters for the Lime application.
	// Params Lime.Enum.DriverType driver
	// Params Lime.Enum.DriverType driver, Vec2 windowSize
	// Params Lime.Enum.DriverType driver, Vec2 windowSize, Vec2 renderSize
	// Returns boolean
	module.set_function("setInitConfig",
		sol::overload(
			sol::resolve<bool(int)>(&Module::Lime::Bind::SetInitConfig),
			sol::resolve<bool(int, const Vec2&)>(&Module::Lime::Bind::SetInitConfig),
			sol::resolve<bool(int, const Vec2&, const Vec2&)>(&Module::Lime::Bind::SetInitConfig)
		));

	// Returns the elapsed time the application has been running in milliseconds.
	// Returns number
	module.set_function("getElapsedTime", &Module::Lime::Bind::GetElapsedTime);

	// Returns the elapsed time the application has been running in seconds.
	// Returns number
	module.set_function("getElapsedTimeSeconds", &Module::Lime::Bind::GetElapsedTimeSeconds);

	// Returns the frame rate in frames per second.
	// Returns number
	module.set_function("getFrameRate", &Module::Lime::Bind::GetFrameRate);

	// Sets the target frame rate in frames per second.
	// Params number target
	// Returns void
	module.set_function("setFrameRate", &Module::Lime::Bind::SetFrameRate);

	// Sets vertical syncronization, matching the frame rate to the current monitor's refresh rate.
	// Params boolean vSyncOn
	// Returns void
	module.set_function("setVSync", &Module::Lime::Bind::SetVSync);

	// Returns true if vertical syncronization is on.
	// Returns boolean
	module.set_function("getVSync", &Module::Lime::Bind::GetVSync);

	// Returns the driver type.
	// Returns Lime.Enum.DriverType
	module.set_function("getDriverType", &Module::Lime::Bind::GetDriverType);

	// [-] Sets debug console configuration. If `enable` is true, the debug console will appear alongside the application. If `writeOutput` is true, the console's output will be written to a output.log file in the application directory. If `suppressWarnings" is true, all warnings will not be logged in the debug console to reduce potential clutter while testing. **Warnings should not be suppressed in shipped applications.**
	// Params boolean enable, boolean? writeOutput, boolean? suppressWarnings
	// Returns void
	module.set_function("setDebugConfig", &Module::Lime::Bind::SetDebugConfig);

	// Returns the application's memory usage in megabytes.
	// Returns number
	module.set_function("getMemoryUsage", &Module::Lime::Bind::GetMemoryUsage);

	// Executes `cmd` in the system's command line.
	// Params string cmd
	// Returns number
	module.set_function("executeCommandLine", &Module::Lime::Bind::ExecuteCommandLine);

	// Returns the value of `arg` from the commmand line, if any. Returns `nil` if no such argument exists.
	// Params string arg
	// Returns string
	module.set_function("getCommandLineArg", &Module::Lime::Bind::GetCommandLineEntry);

	// Displays a pop-up message.
	// Params string title, string message, Lime.Enum.PopUpIcon? icon
	// Returns void
	module.set_function("displayMessage", &Module::Lime::Bind::DisplayMessage);

	// Loads an archive of assets to the application. Content is accessed as if it were at the application's root. For example: If the archive contains folder/image.png, the path ./folder/image.png is valid for loading `Texture` objects.
	// Params string path
	// Returns boolean
	module.set_function("loadArchive", &Module::Lime::Bind::AddArchive);

	a->LimeInit = std::make_shared<Event>();
	a->LimeStart = std::make_shared<Event>();
	a->LimeUpdate = std::make_shared<Event>(); // Call with dt
	a->LimeEnd = std::make_shared<Event>(); // Call with bool isError?

	// Field Event onInit, Event called by Lime prior to initializing the window.
	module["onInit"] = a->LimeInit;

	// Field Event onStart, Event called by Lime following window creation and rendering services are available.
	module["onStart"] = a->LimeStart;

	// Field Event onUpdate, Event called by Lime every rendering frame. This Event is run with a number `dt` argument.
	// Params number dt
	module["onUpdate"] = a->LimeUpdate;

	// Field Event onClose, Event called by Lime once the application closes in any way.
	module["onClose"] = a->LimeEnd;

	// End Module
}

// Functions

void Module::Lime::Bind::SetDebugConfig(bool on, bool write, bool suppress) {
	a->setDebugConfig(on, write, suppress);
}

void Module::Lime::Bind::ClearDebug() {
	d->ClearConsole();
}

void Module::Lime::Bind::Log(std::string msg, int color) {
	d->Log(msg.c_str(), static_cast<MESSAGE_TYPE>(color));
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
	a->setVSync(false);
}

bool Module::Lime::Bind::GetVSync() {
	return a->getVSync();
}

void Module::Lime::Bind::SetVSync(bool on) {
	a->setVSync(on);
}

int Module::Lime::Bind::GetMemoryUsage() {
	return a->getMemoryUsage();
}

int Module::Lime::Bind::ExecuteCommandLine(const std::string& cmd) {
	return std::system(cmd.c_str());
}

sol::object Module::Lime::Bind::GetCommandLineEntry(const std::string& key) {
	return a->getCommandLineValue(key);
}

void Module::Lime::Bind::DisplayMessage(const std::string& title, const std::string message, int img) {
	return a->displayMessage(title, message, img);
}

bool Module::Lime::Bind::AddArchive(const std::string& path) {
	return a->addArchive(path);
}

bool Module::Lime::Bind::SetInitConfig(int driverType) {
	return SetInitConfig(driverType, Vec2(640, 480), Vec2(640, 480));
}

bool Module::Lime::Bind::SetInitConfig(int driverType, const Vec2& windowSize) {
	return SetInitConfig(driverType, windowSize, windowSize);
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
	cfg.stencil = false; // Temporarily

	a->SetConfig(cfg);
	return true;
}

int Module::Lime::Bind::GetDriverType() {
	return a->GetConfig().driverType;
}

int Module::Lime::Bind::GetElapsedTime() {
	return r->getElapsedTime();
}

float Module::Lime::Bind::GetElapsedTimeSeconds() {
	return r->getDtTime();
}
