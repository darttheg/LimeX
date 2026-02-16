#include "Receiver.h"
#include "Application.h"
#include "DebugConsole.h"
#include "Objects/Event.h"

static Application* a;
static DebugConsole* d;

Receiver::Receiver(Application* app) {
	a = app;
	d = a->GetDebugConsole();

	keyboard.down.fill(false);
	keyboard.pressed.fill(false);
	keyboard.released.fill(false);
	keyboard.repeat.fill(false);

	// Events
}

const std::string& Receiver::getText() const {
	if (text.typed.empty()) return "";

	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, text.typed.data(), (int)text.typed.size(), nullptr, 0, nullptr, nullptr);
	std::string out(sizeNeeded, 0);

	WideCharToMultiByte(CP_UTF8, 0, text.typed.data(), (int)text.typed.size(), out.data(), sizeNeeded, nullptr, nullptr);

	return out;
}

void Receiver::beginFrame() {
	keyboard.pressed.fill(false);
	keyboard.released.fill(false);
	keyboard.repeat.fill(false);

	mouse.lmbPressed = mouse.lmbReleased = false;
	mouse.rmbPressed = mouse.rmbReleased = false;
	mouse.mPressed = mouse.mReleased = false;

	mouse.delta = { 0, 0 };
	mouse.wheel = 0.0f;

	if (skipDeltaOnResize) {
		skipDeltaOnResize = false;
		syncMouse();
		return;
	}

	if (firstMouse) {
		syncMouse();
		return;
	}

	mouse.delta.x = mouse.pos.x - lastMousePos.x;
	mouse.delta.y = mouse.pos.y - lastMousePos.y;

	text.typed.clear();
}

void Receiver::endFrame() {
	mouse.lastPos = mouse.pos;
	firstMouse = false;
}

void Receiver::syncMouse() {
	mouse.lastPos = mouse.pos;
	mouse.delta = { 0, 0 };
	firstMouse = false;
}

void Receiver::setMousePosition(int x, int y) {
	mouse.pos.x = x;
	mouse.pos.y = y;
}

bool Receiver::OnEvent(const irr::SEvent& e) {
	switch (e.EventType) {
		case irr::EET_KEY_INPUT_EVENT:
			handleKey(e.KeyInput);
			return false;
		case irr::EET_MOUSE_INPUT_EVENT:
			handleMouse(e.MouseInput);
			return false;
		case irr::EET_JOYSTICK_INPUT_EVENT:
			handleJoystick(e.JoystickEvent);
			return false;
		default: return false;
	}
}

void Receiver::handleKey(const irr::SEvent::SKeyInput& k) {
	int idx = (int)k.Key;
	bool wasDown = keyboard.down[idx];
	bool nowDown = k.PressedDown;

	keyboard.down[idx] = nowDown;

	if (!wasDown && nowDown) keyboard.pressed[idx] = true;
	if (wasDown && !nowDown) keyboard.released[idx] = true;
	keyboard.repeat[idx] = nowDown && wasDown;

	keyboard.shift = k.Shift;
	keyboard.control = k.Control;
	keyboard.alt = keyboard.down[irr::KEY_MENU];

	if (k.PressedDown && k.Char != 0)
		text.typed.push_back(k.Char);
}

void Receiver::handleMouse(const irr::SEvent::SMouseInput& m) {
	Vec2S newPos{ m.X, m.Y };
	mouse.pos = newPos;

	if (!haveLastMousePos) {
		lastMousePos = newPos;
		haveLastMousePos = true;
	}

	mouse.delta.x += (newPos.x - lastMousePos.x);
	mouse.delta.y += (newPos.y - lastMousePos.y);
	lastMousePos = newPos;

	switch (m.Event) {
	case irr::EMIE_LMOUSE_PRESSED_DOWN: // LMB pressed
		if (!mouse.lmbDown) mouse.lmbPressed = true;
		mouse.lmbDown = true;
		break;
	case irr::EMIE_LMOUSE_LEFT_UP: // LMB released
		if (mouse.lmbDown) mouse.lmbReleased = true;
		mouse.lmbDown = false;
		break;
	case irr::EMIE_RMOUSE_PRESSED_DOWN: // RMB pressed
		if (!mouse.rmbDown) mouse.rmbPressed = true;
		mouse.rmbDown = true;
		break;
	case irr::EMIE_RMOUSE_LEFT_UP: // RMB released
		if (mouse.rmbDown) mouse.rmbReleased = true;
		mouse.rmbDown = false;
		break;
	case irr::EMIE_MMOUSE_PRESSED_DOWN: // M pressed
		if (!mouse.mDown) mouse.mPressed = true;
		mouse.mDown = true;
		break;
	case irr::EMIE_MMOUSE_LEFT_UP: // M released
		if (mouse.mDown) mouse.mReleased = true;
		mouse.mDown = false;
		break;
	case irr::EMIE_MOUSE_WHEEL: // M scroll
		mouse.wheel += m.Wheel;
		break;
	}
}

void Receiver::handleJoystick(const irr::SEvent::SJoystickEvent& j) {
}
