#include "Modules/ModuleInput.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Receiver.h"
#include "Renderer.h"
#include "Window.h"

#include "Objects/Event.h"
#include "Objects/Vec2.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Receiver* receiver;
static Renderer* renderer;
static Window* w;

void Module::Input::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	receiver = app->GetReceiver();
	renderer = app->GetRenderer();
	w = app->GetWindow();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Input
	sol::table module = lua["Lime"]["Input"].get_or_create<sol::table>();

	receiver->InputKeyPressed = std::make_shared<Event>();
	receiver->InputKeyReleased = std::make_shared<Event>();
	receiver->InputMouseButtonPressed = std::make_shared<Event>();
	receiver->InputMouseButtonReleased = std::make_shared<Event>();
	receiver->InputMouseMoved = std::make_shared<Event>();
	receiver->InputMouseWheel = std::make_shared<Event>();

	// Field Event onKeyPressed, Event called by Lime when a key is pressed.
	// Params Lime.Enum.Key key
	module["onKeyPressed"] = receiver->InputKeyPressed;

	// Field Event onKeyReleased, Event called by Lime when a key is released.
	// Params Lime.Enum.Key key
	module["onKeyReleased"] = receiver->InputKeyReleased;

	// Field Event onMouseButtonPressed, Event called by Lime when a mouse button is pressed.
	// Params Lime.Enum.Mouse button
	module["onMouseButtonPressed"] = receiver->InputMouseButtonPressed;

	// Field Event onMouseButtonReleased, Event called by Lime when a mouse button is released.
	// Params Lime.Enum.Mouse button
	module["onMouseButtonReleased"] = receiver->InputMouseButtonReleased;

	// Field Event onMouseMoved, Event called by Lime when a mouse is moved.
	// Params Vec2 delta
	module["onMouseMoved"] = receiver->InputMouseMoved;

	// Field Event onMouseScroll, Event called by Lime when the mouse scroll wheel is moved.
	// Params number delta
	module["onMouseScroll"] = receiver->InputMouseWheel;

	// Returns true if `key` is currently pressed.
	// Params Lime.Enum.Key key
	// Returns boolean
	module.set_function("isKeyDown", &Module::Input::Bind::isKeyDown);

	// Returns true if `button` is currently pressed.
	// Params Lime.Enum.Mouse button
	// Returns boolean
	module.set_function("isMouseButtonDown", &Module::Input::Bind::isMouseButtonDown);

	// Returns the change in mouse position within the last frame.
	// Returns Vec2
	module.set_function("getMouseDelta", &Module::Input::Bind::getMouseDelta);

	// Returns the mouse's position.
	// Returns Vec2
	module.set_function("getMousePosition", &Module::Input::Bind::getMousePosition);

	// Sets the mouse's position.
	// Params Vec2 pos
	// Returns void
	module.set_function("setMousePosition", &Module::Input::Bind::setMousePosition);

	// Set the visibility of the mouse cursor.
	// Params boolean visible
	// Returns void
	module.set_function("setMouseVisible", &Module::Input::Bind::setMouseVisible);

	receiver->InputJoystickConnect = std::make_shared<Event>();
	receiver->InputJoystickDisconnect = std::make_shared<Event>();
	receiver->InputJoystickButtonPressed = std::make_shared<Event>();
	receiver->InputJoystickButtonReleased = std::make_shared<Event>();

	// Field Event onControllerConnected, Event called by Lime when a controller is connected.
	// Params number id
	module["onControllerConnected"] = receiver->InputJoystickConnect;

	// Field Event onControllerDisconnected, Event called by Lime when a controller is disconnected.
	// Params number id
	module["onControllerDisconnected"] = receiver->InputJoystickDisconnect;

	// Field Event onControllerButtonPressed, Event called by Lime when a controller button is pressed.
	// Params number id, Lime.Enum.Controller button
	module["onControllerButtonPressed"] = receiver->InputJoystickButtonPressed;

	// Field Event onControllerButtonReleased, Event called by Lime when a controller button is released.
	// Params number id, Lime.Enum.Controller button
	module["onControllerButtonReleased"] = receiver->InputJoystickButtonReleased;

	// Returns true if controller with id `id` has button `button` pressed down.
	// Params number id, Lime.Enum.Controller button
	// Returns boolean
	module.set_function("isButtonDown", &Module::Input::Bind::isButtonDown);

	// Returns controller with id `id`'s `axis` axis value from -1.0 to 1.0. Triggers are ranged 0.0 to 1.0.
	// Params number id, Lime.Enum.ControllerAxis axis
	// Returns number
	module.set_function("getControllerAxis", &Module::Input::Bind::getControllerAxis);

	// Returns true if controller with id `id` is connected.
	// Params number id
	// Returns boolean
	module.set_function("isControllerConnected", &Module::Input::Bind::isControllerConnected);

	// Returns the name of the controller with id `id`.
	// Params number id
	// Returns string
	module.set_function("getControllerName", &Module::Input::Bind::getControllerName);

	// Determines if the mouse is confined to the window or not.
	// Params boolean confined
	// Returns void
	module.set_function("setMouseConfined", &Module::Input::Bind::setMouseLocked);

	// End Module
}

// Functions

bool Module::Input::Bind::isKeyDown(int key) {
	return receiver->isDown(static_cast<Receiver::Key>(key));
}

bool Module::Input::Bind::isMouseButtonDown(int button) {
	switch (button) {
		case 0: // LMB
			return receiver->getMouseState().lmbDown;
		case 1: // RMB
			return receiver->getMouseState().rmbDown;
		case 2: // M
			return receiver->getMouseState().mDown;
		default:
			return false;
	}
}

Vec2 Module::Input::Bind::getMouseDelta() {
	return Vec2(receiver->getMouseState().delta.x, receiver->getMouseState().delta.y);
}

Vec2 Module::Input::Bind::getMousePosition() {
	return renderer->getMousePosCorrected(receiver->getMouseState().pos.x, receiver->getMouseState().pos.y);
}

void Module::Input::Bind::setMousePosition(const Vec2& pos) {
	if (!renderer->setMousePosition(pos)) {
		d->Warn("The window must be created to edit mouse attributes.");
	}
}

void Module::Input::Bind::setMouseVisible(bool vis) {
	if (!renderer->setMouseVisible(vis)) {
		d->Warn("The window must be created to edit mouse attributes.");
	}
}

bool Module::Input::Bind::isButtonDown(int id, int btn) {
	return receiver->isButtonDown(id, btn);
}

float Module::Input::Bind::getControllerAxis(int id, int axis) {
	return receiver->getControllerAxis(id, axis);
}

bool Module::Input::Bind::isControllerConnected(int id) {
	return receiver->isControllerConnected(id);
}

void Module::Input::Bind::setMouseLocked(bool v) {
	w->setMouseLocked(v);
}

std::string Module::Input::Bind::getControllerName(int id) {
	return receiver->getControllerName(id);
}
