#pragma once
#include <array>
#include <algorithm>
#include <string>

#include "IEventReceiver.h"

class Application;

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

	bool OnEvent(const irr::SEvent& e) override;

	const Mouse& getMouseState() const { return mouse; }
	const Keyboard& getKeyboardState() const { return keyboard; }
	const Text& getText() const { return text; }
	bool isDown(Key k) const { return keyboard.down[(int)k]; }
	bool isPressed(Key k) const { return keyboard.pressed[(int)k]; }
	bool isReleased(Key k) const { return keyboard.released[(int)k]; }
	bool isRepeat(Key k) const { return keyboard.repeat[(int)k]; }

	Vec2S lastMousePos{ 0,0 };
	bool haveLastMousePos = false;
	bool firstMouse = true;
	bool skipDeltaOnResize = false;
private:
	Mouse mouse{};
	Keyboard keyboard{};
	Text text{};

	void handleKey(const irr::SEvent::SKeyInput& k);
	void handleMouse(const irr::SEvent::SMouseInput& m);
	void handleJoystick(const irr::SEvent::SJoystickEvent& j);
};