#pragma once
#include <array>
#include <algorithm>
#include <string>
#include <memory>

#include "IEventReceiver.h"

class Application;
class Event;

namespace irr {
	class IrrlichtDevice;
}
using namespace irr;

class Receiver : public irr::IEventReceiver {
public:
	using Key = irr::EKEY_CODE;

	struct Vec2S { float x, y; };

	struct Mouse {
		Vec2S pos;
		Vec2S lastPos;
		Vec2S delta;
		float wheel = 0.0f;

		bool lmbDown = false;
		bool rmbDown = false;
		bool mDown = false;

		bool lmbPressed = false;
		bool lmbReleased = false;

		bool rmbPressed = false;
		bool rmbReleased = false;

		bool mPressed = false;
		bool mReleased = false;
	};

	struct Keyboard {
		std::array<bool, irr::KEY_KEY_CODES_COUNT> down{};
		std::array<bool, irr::KEY_KEY_CODES_COUNT> pressed{};
		std::array<bool, irr::KEY_KEY_CODES_COUNT> released{};
		std::array<bool, irr::KEY_KEY_CODES_COUNT> repeat{};

		bool shift = false;
		bool control = false;
		bool alt = false;
	};

	struct Text {
		std::basic_string<wchar_t> typed;
	};

	Receiver(Application* app);

	void beginFrame();
	void endFrame();
	void syncMouse();
	void setSkipDelta() { skipDeltaOnResize = true; }
	void setMousePosition(int x, int y);

	bool OnEvent(const irr::SEvent& e) override;

	const Mouse& getMouseState() const { return mouse; }
	const Keyboard& getKeyboardState() const { return keyboard; }
	bool isDown(Key k) const { return keyboard.down[(int)k]; }
	bool isPressed(Key k) const { return keyboard.pressed[(int)k]; }
	bool isReleased(Key k) const { return keyboard.released[(int)k]; }
	bool isRepeat(Key k) const { return keyboard.repeat[(int)k]; }

	Vec2S lastMousePos{ 0,0 };
	bool haveLastMousePos = false;
	bool firstMouse = true;
	bool skipDeltaOnResize = false;

	const void setSkipDeltaMouse(bool v) { skipDeltaOnResize = v; }

	std::shared_ptr<Event> InputKeyPressed = nullptr;
	std::shared_ptr<Event> InputKeyReleased = nullptr;
	std::shared_ptr<Event> InputMouseButtonPressed = nullptr;
	std::shared_ptr<Event> InputMouseButtonReleased = nullptr;
	std::shared_ptr<Event> InputMouseMoved = nullptr;
	std::shared_ptr<Event> InputMouseWheel = nullptr;

	std::shared_ptr<Event> InputJoystickConnect = nullptr;
	std::shared_ptr<Event> InputJoystickDisconnect = nullptr;
	std::shared_ptr<Event> InputJoystickButtonPressed = nullptr;
	std::shared_ptr<Event> InputJoystickButtonReleased = nullptr;

	struct Impl;
	std::unique_ptr<Impl> joystickImpl;
	void initJoysticks(IrrlichtDevice* device);
	void pollNewJoysticks();
	void pollDisconnectedJoysticks();

	bool isButtonDown(int id, int btn);
	float getControllerAxis(int id, int axis);
	bool isControllerConnected(int id);
	std::string getControllerName(int id);
private:
	Mouse mouse{};
	Keyboard keyboard{};
	uint64_t lastPulsedJoysticks = 0;
	const uint64_t pulseFreq = 2500;

	void handleKey(const irr::SEvent::SKeyInput& k);
	void handleMouse(const irr::SEvent::SMouseInput& m);
	void handleJoystick(const irr::SEvent::SJoystickEvent& j);
};