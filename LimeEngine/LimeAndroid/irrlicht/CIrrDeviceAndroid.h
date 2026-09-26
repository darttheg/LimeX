// Based on CIrrDeviceAndroid from Irrlicht's ogl-es branch (zlib license).
// The android_app* is passed in as SIrrlichtCreationParameters::WindowId.

#ifndef __C_IRR_DEVICE_ANDROID_H_INCLUDED__
#define __C_IRR_DEVICE_ANDROID_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_ANDROID_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "ICursorControl.h"

#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <jni.h>

namespace irr
{
	class CIrrDeviceAndroid : public CIrrDeviceStub
	{
	public:
		CIrrDeviceAndroid(const SIrrlichtCreationParameters& param);
		virtual ~CIrrDeviceAndroid();

		virtual bool run();
		virtual void yield();
		virtual void sleep(u32 timeMs, bool pauseTimer = false);
		virtual void setWindowCaption(const wchar_t* text);
		virtual bool present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip);
		virtual bool isWindowActive() const;
		virtual bool isWindowFocused() const;
		virtual bool isWindowMinimized() const;
		virtual void closeDevice();
		virtual void setResizable(bool resize = false);
		virtual void minimizeWindow();
		virtual void maximizeWindow();
		virtual void restoreWindow();
		virtual E_DEVICE_TYPE getType() const;

		bool swapBuffers();
		core::dimension2du getSurfaceSize() const;
		void setSwapInterval(int interval);
		void showSoftKeyboard(bool show);
		bool isClosing() const { return Close; }

	private:
		class CCursorControl : public gui::ICursorControl
		{
		public:
			CCursorControl(CIrrDeviceAndroid* device) : Device(device), IsVisible(true) {}
			virtual void setVisible(bool visible) { IsVisible = visible; }
			virtual bool isVisible() const { return IsVisible; }
			virtual void setPosition(const core::position2d<f32>& pos) { setPosition(pos.X, pos.Y); }
			virtual void setPosition(f32 x, f32 y);
			virtual void setPosition(const core::position2d<s32>& pos) { setPosition(pos.X, pos.Y); }
			virtual void setPosition(s32 x, s32 y) { CursorPos.X = x; CursorPos.Y = y; }
			virtual const core::position2d<s32>& getPosition() { return CursorPos; }
			virtual core::position2d<f32> getRelativePosition();
			virtual void setReferenceRect(core::rect<s32>* rect = 0) {}

			core::position2d<s32> CursorPos;
		private:
			CIrrDeviceAndroid* Device;
			bool IsVisible;
		};

		static void handleAndroidCommand(android_app* app, int32_t cmd);
		static s32 handleInput(android_app* app, AInputEvent* event);
		s32 handleMotion(AInputEvent* event);
		s32 handleKey(AInputEvent* event);
		s32 handleGamepadMotion(AInputEvent* event);
		void postMouse(EMOUSE_INPUT_EVENT type, s32 x, s32 y, f32 wheel = 0.f);
		void postGamepad(s32 slot);
		s32 gamepadSlot(s32 deviceId);
		wchar_t getUnicodeChar(s32 action, s32 keyCode, s32 metaState);
		void postKeyboardInput();

		bool createEGLContext();
		bool createEGLSurface();
		void destroyEGLSurface();
		void destroyEGL();
		void pumpEvents(bool block);
		void createDriver();
		void createKeyMap();
		void checkSurfaceSize();

		android_app* Android;
		EGLDisplay Display;
		EGLConfig Config;
		EGLContext Context;
		EGLSurface Surface;
		EGLint SwapInterval;

		bool Focused;
		bool Paused;
		bool Close;
		bool Gl4esReady;
		core::dimension2du LastSurfaceSize;

		s32 MousePointerId;  // first finger, left button
		s32 SecondPointerId; // second finger, right button
		u32 MouseButtons;

		static const s32 MaxGamepads = 4;
		struct SGamepad
		{
			s32 DeviceId;
			u32 Buttons;
			s16 Axis[SEvent::SJoystickEvent::NUMBER_OF_AXES];
		};
		SGamepad Gamepads[MaxGamepads];

		core::array<EKEY_CODE> KeyMap;

		JNIEnv* JNIEnvAttached;
		jclass KeyEventClass;
		jmethodID KeyEventCtor;
		jmethodID KeyEventGetUnicodeChar;
	};

} // end namespace irr

#endif // _IRR_COMPILE_WITH_ANDROID_DEVICE_
#endif // __C_IRR_DEVICE_ANDROID_H_INCLUDED__
