#include "Modules/ModuleLime.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"

#include "Objects/Event.h"

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
	bindEnums(lua, module);

	// Prints a message to console.
	// string msg, Lime.PrintColor? color
	// Returns void
	module.set_function("Log", &Module::Lime::Bind::Log);

	// If set to true, Lime will close on any error. A pop-up will be disclosed prior with error details.
	// boolean doEnd
	module.set_function("SetEndOnError", &Module::Lime::Bind::SetEndOnError);

	// Closes the Lime application.
	module.set_function("Close", &Module::Lime::Bind::Close);

	// Returns the Lime version running.
	// Returns string
	module.set_function("GetVersion", &Module::Lime::Bind::GetVersion);

	module = lua["Lime"]["Events"].get_or_create<sol::table>();
	a->LimeInit = std::make_shared<Event>(); // Call with mutable table
	a->LimeUpdate = std::make_shared<Event>(); // Call with dt
	a->LimeEnd = std::make_shared<Event>(); // Call with bool isError?

	// Field Event Init, Event called by Lime prior to initializing the window. This Event is run with a mutable table argument. Edit this table to change driver type, window size, and more.
	// Field Event Update, Event called by Lime every rendering frame. This Event is run with a number delta time argument.
	// Field Event End, Event called by Lime once the application ends in any way. This Event is run with a boolean didFail argument, true if the closure is because of an error.
	module["Init"] = a->LimeInit;
	module["Update"] = a->LimeUpdate;
	module["End"] = a->LimeEnd;

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