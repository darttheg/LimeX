#include "ModuleLime.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
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

void Module::Lime::bind(sol::state& lua, Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();

	sol::table module = lua["Lime"].get_or_create<sol::table>();
	bindEnums(lua, module);

	module.set_function("Log", &Module::Lime::Bind::Log);
	module.set_function("SetEndOnError", &Module::Lime::Bind::SetEndOnError);
	module.set_function("Close", &Module::Lime::Bind::Close);
	module.set_function("GetVersion", &Module::Lime::Bind::GetVersion);
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