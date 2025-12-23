#include "ModuleLime.h"

#include "DebugConsole.h"
#include "Application.h"
#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;

void Module::Lime::init(DebugConsole* deb, Application* app) {
	if (!deb) return;
	if (!app) return;

	d = deb;
	a = app;
}

void Module::Lime::bind(sol::state& lua) {
	sol::table module = lua["Lime"].get_or_create<sol::table>();

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

#define LIME_FUNC(returnType, name, ...) \
		module.set_function(#name, &Module::Lime::name);

#include "Enums.api.inl"
#include "Lime.api.inl"

#undef LIME_FUNC
#undef LIME_DOC
#undef LIME_ENUM_VALUE
#undef LIME_ENUM
#undef LIME_MODULE
}

// Functions

void Module::Lime::Log(std::string msg, int color) {
	d->Log(msg.c_str(), (MESSAGE_TYPE)color);
}

void Module::Lime::DisplayMessage(std::string msg, std::string title, int icon) {
	a->DisplayMessage(msg, title, icon);
}

void Module::Lime::SetEndOnError(bool v) {
	d->SetEndOnError(v);
}

void Module::Lime::Close() {
	a->EndApp();
}

std::string Module::Lime::GetVersion() {
	return LIME_VERSION;
}

int Module::Lime::GetTime() {
	return 0;
}

float Module::Lime::GetDeltaTime() {
	return 0.0f;
}

int Module::Lime::GetFrameRate() {
	return 0;
}

void Module::Lime::SetFrameRate(int limit) {
}

std::string Module::Lime::GetPlatform() {
	return std::string();
}

int Module::Lime::GetMemoryUsage() {
	return d->GetMemUsed();
}
