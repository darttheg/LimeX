#include "ModuleLime.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;

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

void Lime::bind(sol::state& lua, Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();

	sol::table module = lua["Lime"].get_or_create<sol::table>();
	bindEnums(lua, module);

	module.set_function("Log", &Lime::Bind::Log);
	module.set_function("SetEndOnError", &Lime::Bind::SetEndOnError);
	module.set_function("Close", &Lime::Bind::Close);
	module.set_function("GetVersion", &Lime::Bind::GetVersion);
}

// Functions

void Lime::Bind::Log(std::string msg, int color) {
	d->Log(msg.c_str(), (MESSAGE_TYPE)color);
} 

void Lime::Bind::SetEndOnError(bool v) {
	d->SetEndOnError(v);
}

void Lime::Bind::Close() {
	a->EndApp();
}

std::string Lime::Bind::GetVersion() {
	return LIME_VERSION;
}