#pragma once
#include <vector>
#include <memory>
#include "sol/sol.hpp"

class Application;

namespace EventAndHook {
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

	template <class... Args>
	void engineRun(Args&&... args);
};

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