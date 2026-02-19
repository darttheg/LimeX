#include "Modules/ModuleInput.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Receiver.h"
#include "Renderer.h"

#include "Objects/Event.h"
#include "Objects/Vec2.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Receiver* receiver;
static Renderer* renderer;

void Module::Input::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	receiver = app->GetReceiver();
	renderer = app->GetRenderer();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Input
	sol::table module = lua["Lime"]["Input"].get_or_create<sol::table>();

	receiver->InputKeyPressed = std::make_shared<Event>();
	receiver->InputKeyReleased = std::make_shared<Event>();
	receiver->InputMouseButtonPressed = std::make_shared<Event>();
	receiver->InputMouseButtonReleased = std::make_shared<Event>();
	receiver->InputMouseMoved = std::make_shared<Event>();
	receiver->InputMouseWheel = std::make_shared<Event>();

	// Field Event onKeyPressed, Event called by Lime when a key is pressed. This Event is run with a Lime.Enum.Key `key` argument.
	// Field Event onKeyReleased, Event called by Lime when a key is released. This Event is run with a Lime.Enum.Key `key` argument.
	// Field Event onMouseButtonPressed, Event called by Lime when a mouse button is pressed. This Event is run with a Lime.Enum.Mouse `button` argument.
	// Field Event onMouseButtonReleased, Event called by Lime when a mouse button is released. This Event is run with a Lime.Enum.Mouse `button` argument.
	// Field Event onMouseMoved, Event called by Lime when a mouse is moved. This Event is run with a Vec2 `delta` argument.
	// Field Event onMouseScroll, Event called by Lime when the mouse scroll wheel is moved. This Event is run with a number `delta` argument.

	module["onKeyPressed"] = receiver->InputKeyPressed;
	module["onKeyReleased"] = receiver->InputKeyReleased;
	module["onMouseButtonPressed"] = receiver->InputMouseButtonPressed;
	module["onMouseButtonReleased"] = receiver->InputMouseButtonReleased;
	module["onMouseMoved"] = receiver->InputMouseMoved;
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

	// Field Event onControllerConnected, Event called by Lime when a controller is connected. This Event is run with a number `id` argument.
	// Field Event onControllerDisconnected, Event called by Lime when a controller is disconnected. This Event is run with a number `id` argument.
	// Field Event onControllerButtonPressed, Event called by Lime when a controller button is pressed. This Event is run with number `id`, Lime.Enum.Controller `button` arguments.
	// Field Event onControllerButtonReleased, Event called by Lime when a controller button is released. This Event is run with number `id`, Lime.Enum.Controller `button` arguments.

	module["onControllerConnected"] = receiver->InputJoystickConnect;
	module["onControllerDisconnected"] = receiver->InputJoystickDisconnect;
	module["onControllerButtonPressed"] = receiver->InputJoystickButtonPressed;
	module["onControllerButtonReleased"] = receiver->InputJoystickButtonReleased;

	// Returns true if `button` is currently pressed.
	// Params number id, Lime.Enum.Controller button
	// Returns boolean
	module.set_function("isButtonDown", &Module::Input::Bind::isButtonDown);

	// Returns the controller `axis` axis value from -1.0 to 1.0 from controller `id`.
	// Params number id, Lime.Enum.ControllerAxis axis
	// Returns number
	module.set_function("getControllerAxis", &Module::Input::Bind::getControllerAxis);

	// Returns true if controller `id` is connected.
	// Params number id
	// Returns boolean
	module.set_function("isControllerConnected", &Module::Input::Bind::isControllerConnected);

	// Returns the name of the controller of `id`.
	// Params number id
	// Returns string
	module.set_function("getControllerName", &Module::Input::Bind::getControllerName);

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
	return Vec2(receiver->getMouseState().pos.x, receiver->getMouseState().pos.y);
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

std::string Module::Input::Bind::getControllerName(int id) {
	return receiver->getControllerName(id);
}
