#include "ModuleLime.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;

void Module::Lime::init(DebugConsole* deb, Application* app, Renderer* rend) {
	if (!deb) return;
	if (!app) return;
	if (!rend) return;

	d = deb;
	a = app;
	r = rend;
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
#include "Renderer.h"

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
	return r->GetElapsedTime();
}

float Module::Lime::GetDeltaTime() {
	return r->GetDeltaTime();
}

int Module::Lime::GetFrameRate() {
	return r->GetFrameRate();
}

void Module::Lime::SetFrameLimit(int limit) {
	r->SetFrameLimit(limit);
}

int Module::Lime::GetMemoryUsage() {
	return d->GetMemUsed();
}

void Module::Lime::SetVSync(bool v) {
	r->SetVSync(v);
}

void Module::Lime::SetShowDebugger(bool v) {
	d->SetEnable(v);
}

void Module::Lime::SetWriteDebugOutput(bool v) {
	d->SetWriteOutput(v);
}