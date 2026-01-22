#pragma once
#include <vector>
#include <memory>
#include "sol/sol.hpp"

class Application;

namespace Object::EventBind {
	void bind(Application* app);
}

class Event : public std::enable_shared_from_this<Event> {
private:
	std::vector<int> funcs;

	friend class Hook;
	bool removeRef(int ref);
public:
	Event();

	Hook hook(sol::function f); // Add a function
	void clear(); // Clear list
	void run(); // Run Event
	int getSize() const { return (int)funcs.size(); }
	bool empty();

	template<class ...Args>
	void engineRun(lua_State* L, std::function<void(const std::string&)> onError, Args&&... args);
};

template<class... Args>
inline void Event::engineRun(lua_State* L, std::function<void(const std::string&)> onError, Args&&... args) {
	for (int ref : funcs) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		(sol::stack::push(L, std::forward<Args>(args)), ...);
		if (lua_pcall(L, sizeof...(Args), 0, 0) != LUA_OK) {
			size_t n = 0;
			const char* s = luaL_tolstring(L, -1, &n);
			std::string msg(s, n);
			lua_pop(L, 1);
			if (onError) onError(msg);
		}
	}
}

class Hook {
private:
	std::weak_ptr<Event> myEvent; // Weak pointer so if the Event dies, it won't crash
	int ref = -2; // LUA_NOREF
	bool hooked = false; // True until :unhook()
public:
	Hook();
	Hook(std::shared_ptr<Event> e, int r) : myEvent(e), ref(r), hooked(true) {}

	sol::object unhook();
	bool isHooked() const { return hooked; }
};