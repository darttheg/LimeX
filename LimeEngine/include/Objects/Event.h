#pragma once
#include <vector>
#include <memory>
#include <sol/sol.hpp>
#include <functional>

extern "C" { struct lua_State; }
extern lua_State* ls;

namespace Object::EventBind {
	void bind(lua_State* l);
}

class Event : public std::enable_shared_from_this<Event> {
private:
	std::vector<int> funcs;
	std::vector<int> pendingRemove;
	bool running = false;

	friend class Hook;
	bool removeRef(int ref);

	std::function<void()> onLengthChanged;
	int lastLen = 0;
	void updateLen();
public:
	Event();

	Hook hook(sol::function f); // Add a function
	void clear(); // Clear list
	void run(); // Run Event
	int getSize() const { return (int)funcs.size(); }
	bool empty();

	void setOnLengthChanged(std::function<void()> cb) {
		onLengthChanged = std::move(cb);
		lastLen = (int)funcs.size();
	}

	template<class ...Args>
	void engineRun(std::function<void(const std::string&)> onError, Args&&... args);
};

template<class... Args>
inline void Event::engineRun(std::function<void(const std::string&)> onError, Args&&... args) {
	running = true;
	for (int ref : funcs) {
		if (ref == LUA_NOREF) continue;

		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
		(sol::stack::push(ls, std::forward<Args>(args)), ...);
		if (lua_pcall(ls, sizeof...(Args), 0, 0) != LUA_OK) {
			size_t n = 0;
			const char* s = luaL_tolstring(ls, -1, &n);
			std::string msg(s, n);
			lua_pop(ls, 1);
			if (onError) onError(msg);
		}
	}
	running = false;

	for (int ref : pendingRemove) {
		luaL_unref(ls, LUA_REGISTRYINDEX, ref);
		funcs.erase(std::find(funcs.begin(), funcs.end(), LUA_NOREF));
	}

	if (!pendingRemove.empty()) {
		pendingRemove.clear();
		updateLen();
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