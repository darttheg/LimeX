#include "Receiver.h"
#include "Application.h"
#include "GUIManager.h"
#include "DebugConsole.h"
#include "Objects/Event.h"
#include "Objects/Vec2.h"
#include "Window.h"

#include <Xinput.h>

#include "irrlicht.h"

static Application* a;
static DebugConsole* d;
static GUIManager* g;
static Window* w;
static IrrlichtDevice* device;

struct Receiver::Impl {
	static constexpr int MAX_CONTROLLERS = XUSER_MAX_COUNT;
	XINPUT_STATE state[MAX_CONTROLLERS]{};
	bool connected[MAX_CONTROLLERS]{};
	WORD prevButtons[MAX_CONTROLLERS]{};
};

static WORD ButtonToXFlag(int btn) {
	switch (btn) {
	case 0: return XINPUT_GAMEPAD_A;
	case 1: return XINPUT_GAMEPAD_B;
	case 2: return XINPUT_GAMEPAD_X;
	case 3: return XINPUT_GAMEPAD_Y;
	case 4: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case 5: return XINPUT_GAMEPAD_LEFT_SHOULDER;
	case 6: return XINPUT_GAMEPAD_BACK;
	case 7: return XINPUT_GAMEPAD_START;
	case 8: return XINPUT_GAMEPAD_RIGHT_THUMB;
	case 9: return XINPUT_GAMEPAD_LEFT_THUMB;
	case 32: return XINPUT_GAMEPAD_DPAD_UP;
	case 33: return XINPUT_GAMEPAD_DPAD_RIGHT;
	case 34: return XINPUT_GAMEPAD_DPAD_DOWN;
	case 35: return XINPUT_GAMEPAD_DPAD_LEFT;
	default: return 0;
	}
}

Receiver::Receiver(Application* app, GUIManager* gui) {
	a = app;
	w = app->GetWindow();
	d = a->GetDebugConsole();
	g = gui;
	keyboard.down.fill(false);
	keyboard.pressed.fill(false);
	keyboard.released.fill(false);
	keyboard.repeat.fill(false);

	joystickImpl = std::make_unique<Impl>();
}

void Receiver::beginFrame() {
	keyboard.pressed.fill(false);
	keyboard.released.fill(false);
	keyboard.repeat.fill(false);

	mouse.lmbPressed = mouse.lmbReleased = false;
	mouse.rmbPressed = mouse.rmbReleased = false;
	mouse.mPressed = mouse.mReleased = false;

	mouse.wheel = 0.0f;

	double x, y;
	w->syncMouse(&x, &y);
	mouse.pos.x = x;
	mouse.pos.y = y;

	if (skipDeltaOnResize) {
		skipDeltaOnResize = false;
		syncMouse();
		return;
	}

	if (firstMouse) {
		syncMouse();
		return;
	}

	mouse.delta.x = mouse.pos.x - mouse.lastPos.x;
	mouse.delta.y = mouse.pos.y - mouse.lastPos.y;
}

void Receiver::endFrame() {
	mouse.lastPos = mouse.pos;
	firstMouse = false;

	pollJoystickInput();
}

void Receiver::syncMouse() {
	double x, y;
	w->syncMouse(&x, &y);
	mouse.pos.x = x;
	mouse.pos.y = y;
	mouse.lastPos = mouse.pos;
	mouse.delta = { 0, 0 };
	firstMouse = false;
}

void Receiver::setMousePosition(int x, int y) {
	/*
	mouse.pos.x = x;
	mouse.pos.y = y;
	mouse.lastPos.x = x;
	mouse.lastPos.y = y;
	*/

	syncMouse();
}

bool Receiver::OnEvent(const irr::SEvent& e) {
	switch (e.EventType) {
		case irr::EET_KEY_INPUT_EVENT:
			handleKey(e.KeyInput);
			return false;
		case irr::EET_MOUSE_INPUT_EVENT:
			handleMouse(e.MouseInput);
			return false;
		case irr::EET_GUI_EVENT:
			handleGUI(e.GUIEvent);
			return false;
		default: return false;
	}
}

void Receiver::handleKey(const irr::SEvent::SKeyInput& k) {
	int idx = (int)k.Key;
	bool wasDown = keyboard.down[idx];
	bool nowDown = k.PressedDown;

	keyboard.down[idx] = nowDown;

	if (!wasDown && nowDown) {
		keyboard.pressed[idx] = true;
		InputKeyPressed.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, idx);
	}

	if (wasDown && !nowDown) {
		keyboard.released[idx] = true;
		InputKeyReleased.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, idx);
	}

	keyboard.repeat[idx] = nowDown && wasDown;

	keyboard.shift = k.Shift;
	keyboard.control = k.Control;
	keyboard.alt = keyboard.down[irr::KEY_MENU];
}

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2
void Receiver::handleMouse(const irr::SEvent::SMouseInput& m) {
	switch (m.Event) {
	case irr::EMIE_LMOUSE_PRESSED_DOWN: // LMB pressed
		if (!mouse.lmbDown) {
			mouse.lmbPressed = true;
			InputMouseButtonPressed.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, MOUSE_LEFT);
		}
		mouse.lmbDown = true;
		break;
	case irr::EMIE_LMOUSE_LEFT_UP: // LMB released
		if (mouse.lmbDown) {
			mouse.lmbReleased = true;
			InputMouseButtonReleased.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, MOUSE_LEFT);
		}
		mouse.lmbDown = false;
		break;
	case irr::EMIE_RMOUSE_PRESSED_DOWN: // RMB pressed
		if (!mouse.rmbDown) {
			mouse.rmbPressed = true;
			InputMouseButtonPressed.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, MOUSE_RIGHT);
		}
		mouse.rmbDown = true;
		break;
	case irr::EMIE_RMOUSE_LEFT_UP: // RMB released
		if (mouse.rmbDown) {
			mouse.rmbReleased = true;
			InputMouseButtonReleased.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, MOUSE_RIGHT);
		}
		mouse.rmbDown = false;
		break;
	case irr::EMIE_MMOUSE_PRESSED_DOWN: // M pressed
		if (!mouse.mDown) {
			mouse.mPressed = true;
			InputMouseButtonPressed.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, MOUSE_MIDDLE);
		}
		mouse.mDown = true;
		break;
	case irr::EMIE_MMOUSE_LEFT_UP: // M released
		if (mouse.mDown) {
			mouse.mReleased = true;
			InputMouseButtonReleased.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, MOUSE_MIDDLE);
		}
		mouse.mDown = false;
		break;
	case irr::EMIE_MOUSE_WHEEL: // M scroll
		mouse.wheel += m.Wheel;
		InputMouseWheel.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, m.Wheel);
		break;
	case irr::EMIE_MOUSE_MOVED:
		InputMouseMoved.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); }, Vec2(mouse.delta.x, mouse.delta.y));
		break;
	}
}

static inline float NormalizeAxisS16(irr::s16 v) {
	if (v <= -32768) return -1.0f;
	return (float)v / 32767.0f;
}

static inline float ApplyDeadzone(float x, float deadzone) {
	const float ax = std::abs(x);
	if (ax <= deadzone) return 0.0f;

	const float sign = (x < 0.0f) ? -1.0f : 1.0f;
	const float t = (ax - deadzone) / (1.0f - deadzone);
	return sign * t;
}

static uint64_t NowMs() {
	using namespace std::chrono;
	static const steady_clock::time_point start = steady_clock::now();
	return (uint64_t)duration_cast<milliseconds>(steady_clock::now() - start).count();
}

void Receiver::pollJoystickInput() {
	for (int i = 0; i < Impl::MAX_CONTROLLERS; ++i) {
		XINPUT_STATE state{};
		const bool isConnected = (XInputGetState((DWORD)i, &state) == ERROR_SUCCESS);

		if (isConnected && !joystickImpl->connected[i]) {
			InputJoystickConnect.get()->engineRun([&](const std::string& msg) {
				d->PostError(msg, false);
			}, i);
		}
		else if (!isConnected && joystickImpl->connected[i]) {
			InputJoystickDisconnect.get()->engineRun([&](const std::string& msg) {
				d->PostError(msg, false);
				}, i);
		}
		joystickImpl->connected[i] = isConnected;
		if (!isConnected) continue;
		const WORD now = state.Gamepad.wButtons;
		const WORD prev = joystickImpl->prevButtons[i];
		const WORD pressedMask = (~prev) & now;
		const WORD releasedMask = prev & (~now);

		static const int allButtons[] = { 0,1,2,3,4,5,6,7,8,9,32,33,34,35 };
		for (int btn : allButtons) {
			const WORD flag = ButtonToXFlag(btn);
			if (pressedMask & flag) {
				InputJoystickButtonPressed.get()->engineRun([&](const std::string& msg) {
					d->PostError(msg, false);
					}, i, btn);
			}
			if (releasedMask & flag) {
				InputJoystickButtonReleased.get()->engineRun([&](const std::string& msg) {
					d->PostError(msg, false);
					}, i, btn);
			}
		}

		joystickImpl->prevButtons[i] = now;
		joystickImpl->state[i] = state;
	}
}

bool Receiver::isButtonDown(int id, int btn) {
	if (id < 0 || id >= Impl::MAX_CONTROLLERS || !joystickImpl->connected[id]) return false;

	const WORD flag = ButtonToXFlag(btn);
	if (!flag) return false;

	return (joystickImpl->state[id].Gamepad.wButtons & flag) != 0;
}

static inline float SnapToOne(float x) {
	float eps = 0.01f;
	if (x >= 1.0f - eps) return 1.0f;
	if (x <= eps) return 0.0f;
	return x;
}

float Receiver::getControllerAxis(int id, int axis) {
	if (id < 0 || id >= Impl::MAX_CONTROLLERS || !joystickImpl->connected[id]) return 0.0f;

	const auto& pad = joystickImpl->state[id].Gamepad;
	const float deadzone = 0.12f;

	switch (axis) {
	case 0: return ApplyDeadzone(NormalizeAxisS16(pad.sThumbLX), deadzone);
	case 1: return ApplyDeadzone(NormalizeAxisS16(pad.sThumbLY), deadzone);
	case 3: return ApplyDeadzone(NormalizeAxisS16(pad.sThumbRX), deadzone);
	case 4: return ApplyDeadzone(NormalizeAxisS16(pad.sThumbRY), deadzone);
	case 5: return SnapToOne(pad.bLeftTrigger / 255.0f);
	case 6: return SnapToOne(pad.bRightTrigger / 255.0f);
	default: return 0.0f;
	}

	// Leave snapping for triggers?
}

bool Receiver::isControllerConnected(int id) {
	if (id < 0 || id >= Impl::MAX_CONTROLLERS) return false;
	return joystickImpl->connected[id];
}

void Receiver::handleGUI(const irr::SEvent::SGUIEvent& ge) {
	g->handleGUIEvent(ge.Caller, ge.Element, ge.EventType);
}