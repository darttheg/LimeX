#include "Objects/Event.h"
#include "Application.h"
#include "DebugConsole.h"

static Application* a;
static DebugConsole* d;

Event::Event() {
}

Hook Event::hook(sol::function f) {
	f.push();
	int ref = luaL_ref(a->GetLuaState(), LUA_REGISTRYINDEX);
	funcs.push_back(ref);
	// We can get the function because it's sitting at the top of the registry after just being called.

	return Hook(shared_from_this(), ref);
}

bool Event::removeRef(int ref) {
	auto it = std::find(funcs.begin(), funcs.end(), ref);
	if (it == funcs.end())
		return false;

	luaL_unref((a->GetLuaState()), LUA_REGISTRYINDEX, ref);
	funcs.erase(it);
	return true;
}

void Event::clear() {
	for (int ref : funcs) {
		luaL_unref(a->GetLuaState(), LUA_REGISTRYINDEX, ref);
	}

	funcs.clear();
}

bool Event::empty() {
	return getSize() == 0;
}

void Event::run() {
	int argc = lua_gettop(a->GetLuaState());
	int passc = (argc >= 1) ? (argc - 1) : 0;

	for (int ref : funcs) {
		lua_rawgeti((a->GetLuaState()), LUA_REGISTRYINDEX, ref); // Push callback function from registry onto stack

		// Starts at index 2 to exclude self
		for (int i = 2; i <= argc; ++i)
			lua_pushvalue(a->GetLuaState(), i);

		// Call Lua function with pushed arguments
		if (lua_pcall(a->GetLuaState(), passc, 0, 0) != LUA_OK)
			lua_pop(a->GetLuaState(), 1);
	}

	if (passc > 0)
		lua_pop(a->GetLuaState(), passc);
}

static void bindEvent() {
	sol::state_view view(a->GetLuaState());
	sol::usertype<Event> bindType = view.new_usertype<Event>("Event",
		sol::no_constructor,
		sol::meta_function::type, [](const Event&) { return "Event"; }
	);

	// Object Event

	// Hook a function to this Event.
	// function Function
	// Returns Hook
	bindType["Hook"] = &Event::hook;

	// Clears all functions hooked to this Event.
	bindType["Clear"] = &Event::clear;

	// Run this Event.
	// any ...
	bindType["Run"] = &Event::run;

	// Returns the number of hooked functions on this Event.
	// Returns number
	bindType["Length"] = &Event::getSize;

	// End Object

	bindType.set_function("new",
		sol::factories([]() {
			return std::make_shared<Event>();
			})
	);

	bindType[sol::meta_function::call] = &Event::run;
}

//// Hook ////

Hook::Hook() {
}

sol::object Hook::unhook() {
	if (!hooked)
		return sol::nil;

	if (auto e = myEvent.lock())
		e->removeRef(ref);

	ref = LUA_NOREF;
	hooked = false;

	return sol::nil;
}

static void bindHook() {
	sol::state_view view(a->GetLuaState());
	sol::usertype<Hook> bindType = view.new_usertype<Hook>("Hook",
		sol::constructors<Hook()>(),
		sol::meta_function::type, [](const Hook&) { return "Hook"; }
	);
	// Only receive this object when hooking to an Event so no constructors

	// Object Hook

	// Unhook a function to this Event.
	bindType["Unhook"] = &Hook::unhook;

	// Returns true if this hook is still hooked to an Event.
	// Returns boolean
	bindType["IsHooked"] = &Hook::isHooked;

	// End Object
}

void EventAndHook::bind(Application* app) {
	a = app;
	d = a->GetDebugConsole();
	bindEvent();
	bindHook();
}