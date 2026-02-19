#include "Receiver.h"
#include "Application.h"
#include "DebugConsole.h"
#include "Objects/Event.h"
#include "Objects/Vec2.h"

#include "irrlicht.h"

static Application* a;
static DebugConsole* d;
static IrrlichtDevice* device;

struct Receiver::Impl {
	irr::core::array<irr::SJoystickInfo> joysticks;
	std::unordered_map<int32_t, irr::SEvent::SJoystickEvent> lastJoystickState;
	std::unordered_map<int32_t, uint64_t> lastSeenMs;
};

Receiver::Receiver(Application* app) {
	a = app;
	d = a->GetDebugConsole();

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
}

void Receiver::endFrame() {
	mouse.lastPos = mouse.pos;
	firstMouse = false;

	pollNewJoysticks();
	pollDisconnectedJoysticks();
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

	if (!wasDown && nowDown) {
		keyboard.pressed[idx] = true;
		InputKeyPressed.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, idx);
	}

	if (wasDown && !nowDown) {
		keyboard.released[idx] = true;
		InputKeyReleased.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, idx);
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
	Vec2S newPos{ m.X, m.Y };
	mouse.pos = newPos;

	if (!haveLastMousePos) {
		lastMousePos = newPos;
		haveLastMousePos = true;
	}

	mouse.delta.x += (newPos.x - lastMousePos.x);
	mouse.delta.y += (newPos.y - lastMousePos.y);

	if (std::abs(mouse.delta.x + mouse.delta.y) >= 0.01f) {
		InputMouseMoved.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, Vec2(mouse.delta.x, mouse.delta.y));
	}

	lastMousePos = newPos;

	switch (m.Event) {
	case irr::EMIE_LMOUSE_PRESSED_DOWN: // LMB pressed
		if (!mouse.lmbDown) {
			mouse.lmbPressed = true;
			InputMouseButtonPressed.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, MOUSE_LEFT);
		}
		mouse.lmbDown = true;
		break;
	case irr::EMIE_LMOUSE_LEFT_UP: // LMB released
		if (mouse.lmbDown) {
			mouse.lmbReleased = true;
			InputMouseButtonReleased.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, MOUSE_LEFT);
		}
		mouse.lmbDown = false;
		break;
	case irr::EMIE_RMOUSE_PRESSED_DOWN: // RMB pressed
		if (!mouse.rmbDown) {
			mouse.rmbPressed = true;
			InputMouseButtonPressed.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, MOUSE_RIGHT);
		}
		mouse.rmbDown = true;
		break;
	case irr::EMIE_RMOUSE_LEFT_UP: // RMB released
		if (mouse.rmbDown) {
			mouse.rmbReleased = true;
			InputMouseButtonReleased.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, MOUSE_RIGHT);
		}
		mouse.rmbDown = false;
		break;
	case irr::EMIE_MMOUSE_PRESSED_DOWN: // M pressed
		if (!mouse.mDown) {
			mouse.mPressed = true;
			InputMouseButtonPressed.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, MOUSE_MIDDLE);
		}
		mouse.mDown = true;
		break;
	case irr::EMIE_MMOUSE_LEFT_UP: // M released
		if (mouse.mDown) {
			mouse.mReleased = true;
			InputMouseButtonReleased.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, MOUSE_MIDDLE);
		}
		mouse.mDown = false;
		break;
	case irr::EMIE_MOUSE_WHEEL: // M scroll
		mouse.wheel += m.Wheel;
		InputMouseWheel.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, m.Wheel);
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

void Receiver::initJoysticks(IrrlichtDevice* dev) {
	if (!dev) return;
	device = dev;
	joystickImpl->joysticks.clear();

	if (!device->activateJoysticks(joystickImpl->joysticks))
		d->Warn("Could not activate controller support: it is unsupported on this device.");
}

void Receiver::pollNewJoysticks() {
	if (!device) return;

	if (true) return; // Could not set DirectInput device cooperative level error, probably because of GLFW ownership?

	const uint64_t t = NowMs();
	if (t - lastPulsedJoysticks < pulseFreq) {
		return;
	}
	lastPulsedJoysticks = t;

	irr::core::array<irr::SJoystickInfo> tmp;
	const bool ok = device->activateJoysticks(tmp);
	if (!ok) return;

	if (tmp.size() == 0 && joystickImpl->joysticks.size() > 0) return;

	const irr::u32 oldC = joystickImpl->joysticks.size();
	const irr::u32 newC = tmp.size();

	joystickImpl->joysticks = tmp;

	if (newC > oldC) {
		for (irr::u32 i = oldC; i < newC; ++i) {
			const int32_t id = (int32_t)i;
			InputJoystickConnect.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id);
		}
	}
}

void Receiver::pollDisconnectedJoysticks() {
	const uint64_t t = NowMs();

	for (auto it = joystickImpl->lastSeenMs.begin(); it != joystickImpl->lastSeenMs.end(); ) {
		const int32_t id = it->first;
		const uint64_t lastSeen = it->second;

		if (t - lastSeen > pulseFreq) {
			InputJoystickDisconnect.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id);

			joystickImpl->lastSeenMs.erase(it++);
			joystickImpl->lastJoystickState.erase(id);
		} else
			++it;
	}
}

bool Receiver::isButtonDown(int id, int btn) {
	if (id < 0 || btn < 0) return false;
	if ((irr::u32)id >= joystickImpl->joysticks.size()) return false;
	auto it = joystickImpl->lastJoystickState.find(id);
	if (it == joystickImpl->lastJoystickState.end()) return false;

	if (btn >= 32) return false;
	return it->second.IsButtonPressed((irr::u32)btn);
}

static inline float SnapToOne(float x) {
	float eps = 0.01f;
	if (x >= 1.0f - eps) return 1.0f;
	if (x <= eps) return 0.0f;
	return x;
}

float Receiver::getControllerAxis(int id, int axis) {
	if (id < 0 || axis < 0) return 0.0f;
	if ((irr::u32)id >= joystickImpl->joysticks.size()) return 0.0f;
	auto it = joystickImpl->lastJoystickState.find(id);
	if (it == joystickImpl->lastJoystickState.end()) return 0.0f;

	// const int maxAxes = irr::SEvent::SJoystickEvent::NUMBER_OF_AXES;
	if (axis > 6) return 0.0f;

	// Triggers
	if (axis == 5 || axis == 6) {
		float out = getControllerAxis(0, irr::SEvent::SJoystickEvent::AXIS_Z);
		bool left = axis == 5;

		if (left) {
			if (out < 0.0f) return 0.0f;
			return SnapToOne(out);
		}
		else {
			if (out > 0.0f) return 0.0f;
			return SnapToOne(out * -1.0f);
		}
	}

	const irr::s16 raw = it->second.Axis[axis];
	float x = NormalizeAxisS16(raw);
	if (std::abs(x) <= 0.001) return 0.0f;
	return NormalizeAxisS16(raw);
}

bool Receiver::isControllerConnected(int id) {
	if (id < 0) return false;
	return (irr::u32)id < joystickImpl->joysticks.size();
}

void Receiver::handleJoystick(const irr::SEvent::SJoystickEvent& j) {
	const int32_t id = (int32_t)j.Joystick;

	const bool firstTime = (joystickImpl->lastJoystickState.find(id) == joystickImpl->lastJoystickState.end());

	if (firstTime) {
		joystickImpl->lastJoystickState[id] = j;
		InputJoystickConnect.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id);
	}

	auto& prevState = joystickImpl->lastJoystickState[id];

	uint32_t btnCount = 32u;
	if (id >= 0 && id < (int32_t)joystickImpl->joysticks.size())
		btnCount = joystickImpl->joysticks[(irr::u32)id].Buttons;

	const uint32_t prev = (uint32_t)prevState.ButtonStates;
	const uint32_t now = (uint32_t)j.ButtonStates;

	const uint32_t pressedMask = (~prev) & now;
	const uint32_t releasedMask = prev & (~now);

	const uint32_t limit = (btnCount >= 32u) ? 32u : btnCount;

	for (uint32_t i = 0; i < limit; ++i)
	{
		const uint32_t bit = 1u << i;

		if (pressedMask & bit)
			InputJoystickButtonPressed.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id, (int)i);

		if (releasedMask & bit)
			InputJoystickButtonReleased.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id, (int)i);
	}

	uint32_t base = 32u;
	auto povTo4 = [](uint16_t pov, bool& up, bool& right, bool& down, bool& left) {
		up = right = down = left = false;
		if (pov == 0xFFFF) return;

		int dir = (int)(((pov + 2250) / 4500) % 8);
		// 0=N,1=NE,2=E,3=SE,4=S,5=SW,6=W,7=NW
		up = (dir == 7 || dir == 0 || dir == 1);
		right = (dir == 1 || dir == 2 || dir == 3);
		down = (dir == 3 || dir == 4 || dir == 5);
		left = (dir == 5 || dir == 6 || dir == 7);
		};

	bool pup, pright, pdown, pleft;
	bool nup, nright, ndown, nleft;
	povTo4(prevState.POV, pup, pright, pdown, pleft);
	povTo4(j.POV, nup, nright, ndown, nleft);

	auto edge = [&](bool was, bool now, int virtualIndex) {
		if (!was && now)
			InputJoystickButtonPressed.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id, virtualIndex);
		if (was && !now)
			InputJoystickButtonReleased.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); }, id, virtualIndex);
		};

	edge(pup, nup, (int)(base + 0)); // POV_Up
	edge(pright, nright, (int)(base + 1)); // POV_Right
	edge(pdown, ndown, (int)(base + 2)); // POV_Down
	edge(pleft, nleft, (int)(base + 3)); // POV_Left

	const int axisCount = (int)j.NUMBER_OF_AXES;
	const float deadzone = 0.12f;
	const float eps = 0.0025f;

	for (int axis = 0; axis < axisCount; ++axis) {
		float prev = ApplyDeadzone(NormalizeAxisS16(prevState.Axis[axis]), deadzone);
		float now = ApplyDeadzone(NormalizeAxisS16(j.Axis[axis]), deadzone);
	}

	joystickImpl->lastJoystickState[id] = j;
	joystickImpl->lastSeenMs[id] = NowMs();
}
