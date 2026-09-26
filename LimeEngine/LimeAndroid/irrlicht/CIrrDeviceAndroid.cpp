#include "CIrrDeviceAndroid.h"

#ifdef _IRR_COMPILE_WITH_ANDROID_DEVICE_

#include "os.h"
#include "IVideoDriver.h"
#include "IGUIEnvironment.h"
#include "ISceneManager.h"
#include "CNullDriver.h"

#include <android/input.h>
#include <android/keycodes.h>
#include <android/native_window.h>
#include <time.h>

#include "gl4esinit.h"
#include "LimeAndroid.h"

static void (*PauseListener)(bool paused) = 0;

namespace irr
{
namespace video
{
	IVideoDriver* createOpenGLDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, CIrrDeviceAndroid* device);
}

// gl4es asks for the default framebuffer size
static CIrrDeviceAndroid* ActiveDevice = 0;
static void getMainFramebufferSize(int* width, int* height)
{
	core::dimension2du size = ActiveDevice ? ActiveDevice->getSurfaceSize() : core::dimension2du(0, 0);
	*width = (int)size.Width;
	*height = (int)size.Height;
}

// Same button numbers as XInput in Receiver.cpp, d-pad on bits 10-13
enum
{
	PAD_A = 0, PAD_B, PAD_X, PAD_Y, PAD_RB, PAD_LB, PAD_BACK, PAD_START, PAD_RSTICK, PAD_LSTICK,
	PAD_DPAD_UP, PAD_DPAD_RIGHT, PAD_DPAD_DOWN, PAD_DPAD_LEFT
};

static s32 gamepadButtonForKey(s32 keyCode)
{
	switch (keyCode)
	{
	case AKEYCODE_BUTTON_A: return PAD_A;
	case AKEYCODE_BUTTON_B: return PAD_B;
	case AKEYCODE_BUTTON_X: return PAD_X;
	case AKEYCODE_BUTTON_Y: return PAD_Y;
	case AKEYCODE_BUTTON_R1: return PAD_RB;
	case AKEYCODE_BUTTON_L1: return PAD_LB;
	case AKEYCODE_BUTTON_SELECT: return PAD_BACK;
	case AKEYCODE_BUTTON_START: return PAD_START;
	case AKEYCODE_BUTTON_THUMBR: return PAD_RSTICK;
	case AKEYCODE_BUTTON_THUMBL: return PAD_LSTICK;
	default: return -1;
	}
}

static s32 dpadButtonForKey(s32 keyCode)
{
	switch (keyCode)
	{
	case AKEYCODE_DPAD_UP: return PAD_DPAD_UP;
	case AKEYCODE_DPAD_RIGHT: return PAD_DPAD_RIGHT;
	case AKEYCODE_DPAD_DOWN: return PAD_DPAD_DOWN;
	case AKEYCODE_DPAD_LEFT: return PAD_DPAD_LEFT;
	default: return -1;
	}
}

static bool isFromGamepad(AInputEvent* event)
{
	const s32 source = AInputEvent_getSource(event);
	return (source & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD
		|| (source & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK;
}

static s16 axisToS16(f32 value)
{
	if (value > 1.f) value = 1.f;
	if (value < -1.f) value = -1.f;
	return (s16)(value * 32767.f);
}

CIrrDeviceAndroid::CIrrDeviceAndroid(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Android(0), Display(EGL_NO_DISPLAY), Config(0), Context(EGL_NO_CONTEXT),
	Surface(EGL_NO_SURFACE), SwapInterval(param.Vsync ? 1 : 0), Focused(true), Paused(false), Close(false),
	Gl4esReady(false), MousePointerId(-1), SecondPointerId(-1), MouseButtons(0),
	JNIEnvAttached(0), KeyEventClass(0), KeyEventCtor(0), KeyEventGetUnicodeChar(0)
{
	#ifdef _DEBUG
	setDebugName("CIrrDeviceAndroid");
	#endif

	for (s32 i = 0; i < MaxGamepads; ++i)
	{
		Gamepads[i].DeviceId = -1;
		Gamepads[i].Buttons = 0;
		for (s32 a = 0; a < SEvent::SJoystickEvent::NUMBER_OF_AXES; ++a)
			Gamepads[i].Axis[a] = 0;
	}

	// 1.8 has no PrivateData, so the android_app comes in as WindowId
	Android = (android_app*)param.WindowId;
	if (!Android)
	{
		os::Printer::log("The Android device needs the android_app in SIrrlichtCreationParameters::WindowId.", ELL_ERROR);
		return;
	}

	Android->userData = this;
	Android->onAppCmd = handleAndroidCommand;
	Android->onInputEvent = handleInput;
	ActiveDevice = this;

	createKeyMap();
	CursorControl = new CCursorControl(this);

	while (!Android->window && !Close)
		pumpEvents(true);

	if (Close || !createEGLContext() || !createEGLSurface())
		return;

	// gl4es is built without its constructor, so init it here.
	// NOBGRA: many GLES drivers can't mipmap BGRA textures and they render blank.
	setenv("LIBGL_NOBGRA", "1", 0);
	set_getmainfbsize(getMainFramebufferSize);
	initialize_gl4es();
	Gl4esReady = true;

	LastSurfaceSize = getSurfaceSize();
	CreationParams.WindowSize = LastSurfaceSize;
	CreationParams.Fullscreen = true;

	createDriver();
	if (VideoDriver)
		createGUIAndScene();
}

CIrrDeviceAndroid::~CIrrDeviceAndroid()
{
	if (GUIEnvironment)
	{
		GUIEnvironment->drop();
		GUIEnvironment = 0;
	}
	if (SceneManager)
	{
		SceneManager->drop();
		SceneManager = 0;
	}
	if (VideoDriver)
	{
		VideoDriver->drop();
		VideoDriver = 0;
	}

	destroyEGL();

	if (Android)
	{
		if (JNIEnvAttached)
		{
			if (KeyEventClass)
				JNIEnvAttached->DeleteGlobalRef(KeyEventClass);
			Android->activity->vm->DetachCurrentThread();
			JNIEnvAttached = 0;
		}
		Android->userData = 0;
		Android->onAppCmd = 0;
		Android->onInputEvent = 0;
	}

	if (ActiveDevice == this)
		ActiveDevice = 0;
}

bool CIrrDeviceAndroid::createEGLContext()
{
	Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (Display == EGL_NO_DISPLAY || !eglInitialize(Display, 0, 0))
	{
		os::Printer::log("Could not initialize EGL.", ELL_ERROR);
		return false;
	}

	// Fall back to fewer buffers if needed
	const EGLint stencilWanted = CreationParams.Stencilbuffer ? 8 : 0;
	const EGLint samplesWanted = CreationParams.AntiAlias > 1 ? CreationParams.AntiAlias : 0;
	struct SAttempt { EGLint depth, stencil, samples; };
	const SAttempt attempts[] = {
		{ 24, stencilWanted, samplesWanted },
		{ 24, stencilWanted, 0 },
		{ 16, stencilWanted, 0 },
		{ 16, 0, 0 },
	};

	EGLint numConfigs = 0;
	for (u32 i = 0; i < sizeof(attempts) / sizeof(attempts[0]); ++i)
	{
		const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
			EGL_DEPTH_SIZE, attempts[i].depth,
			EGL_STENCIL_SIZE, attempts[i].stencil,
			EGL_SAMPLE_BUFFERS, attempts[i].samples ? 1 : 0,
			EGL_SAMPLES, attempts[i].samples,
			EGL_NONE
		};
		if (eglChooseConfig(Display, attribs, &Config, 1, &numConfigs) && numConfigs > 0)
		{
			if (attempts[i].stencil < stencilWanted)
				os::Printer::log("No EGL config with a stencil buffer; shadows are disabled.", ELL_WARNING);
			break;
		}
	}

	if (numConfigs <= 0)
	{
		os::Printer::log("No usable EGL config found.", ELL_ERROR);
		return false;
	}

	const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	Context = eglCreateContext(Display, Config, EGL_NO_CONTEXT, contextAttribs);
	if (Context == EGL_NO_CONTEXT)
	{
		os::Printer::log("Could not create an OpenGL ES 2 context.", ELL_ERROR);
		return false;
	}
	return true;
}

bool CIrrDeviceAndroid::createEGLSurface()
{
	if (!Android->window || Display == EGL_NO_DISPLAY || Context == EGL_NO_CONTEXT)
		return false;
	if (Surface != EGL_NO_SURFACE)
		return true;

	EGLint format = 0;
	eglGetConfigAttrib(Display, Config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(Android->window, 0, 0, format);

	Surface = eglCreateWindowSurface(Display, Config, Android->window, 0);
	if (Surface == EGL_NO_SURFACE)
	{
		os::Printer::log("Could not create the EGL window surface.", ELL_ERROR);
		return false;
	}
	if (!eglMakeCurrent(Display, Surface, Surface, Context))
	{
		os::Printer::log("Could not make the EGL context current.", ELL_ERROR);
		return false;
	}
	eglSwapInterval(Display, SwapInterval);
	return true;
}

void CIrrDeviceAndroid::destroyEGLSurface()
{
	if (Display == EGL_NO_DISPLAY || Surface == EGL_NO_SURFACE)
		return;
	// Keep the context so textures survive
	eglMakeCurrent(Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(Display, Surface);
	Surface = EGL_NO_SURFACE;
}

void CIrrDeviceAndroid::destroyEGL()
{
	if (Display == EGL_NO_DISPLAY)
		return;
	eglMakeCurrent(Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	if (Surface != EGL_NO_SURFACE)
		eglDestroySurface(Display, Surface);
	if (Context != EGL_NO_CONTEXT)
		eglDestroyContext(Display, Context);
	eglTerminate(Display);
	Display = EGL_NO_DISPLAY;
	Context = EGL_NO_CONTEXT;
	Surface = EGL_NO_SURFACE;
}

bool CIrrDeviceAndroid::swapBuffers()
{
	if (Surface == EGL_NO_SURFACE)
		return false;
	if (!eglSwapBuffers(Display, Surface))
	{
		const EGLint err = eglGetError();
		if (err == EGL_CONTEXT_LOST)
			os::Printer::log("The EGL context was lost.", ELL_ERROR);
		return false;
	}
	return true;
}

core::dimension2du CIrrDeviceAndroid::getSurfaceSize() const
{
	if (Surface != EGL_NO_SURFACE)
	{
		EGLint w = 0, h = 0;
		eglQuerySurface(Display, Surface, EGL_WIDTH, &w);
		eglQuerySurface(Display, Surface, EGL_HEIGHT, &h);
		return core::dimension2du((u32)w, (u32)h);
	}
	if (Android && Android->window)
		return core::dimension2du((u32)ANativeWindow_getWidth(Android->window), (u32)ANativeWindow_getHeight(Android->window));
	return core::dimension2du(0, 0);
}

void CIrrDeviceAndroid::setSwapInterval(int interval)
{
	SwapInterval = interval;
	if (Display != EGL_NO_DISPLAY && Surface != EGL_NO_SURFACE)
		eglSwapInterval(Display, SwapInterval);
}

void CIrrDeviceAndroid::showSoftKeyboard(bool show)
{
	if (!Android)
		return;
	if (show)
		ANativeActivity_showSoftInput(Android->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
	else
		ANativeActivity_hideSoftInput(Android->activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
}

void CIrrDeviceAndroid::checkSurfaceSize()
{
	const core::dimension2du size = getSurfaceSize();
	if (size.Width == 0 || size.Height == 0 || size == LastSurfaceSize)
		return;
	LastSurfaceSize = size;
	if (VideoDriver)
		VideoDriver->OnResize(size);
}

void CIrrDeviceAndroid::pumpEvents(bool block)
{
	int timeout = block ? -1 : 0;
	for (;;)
	{
		int events = 0;
		android_poll_source* source = 0;
		const int id = ALooper_pollOnce(timeout, 0, &events, (void**)&source);
		if (id == ALOOPER_POLL_TIMEOUT || id == ALOOPER_POLL_ERROR)
			break;
		if (id >= 0 && source)
			source->process(Android, source);
		if (Android->destroyRequested)
		{
			Close = true;
			break;
		}
		if (block && id >= 0)
			break; // let the caller re-check its condition
	}
}

bool CIrrDeviceAndroid::run()
{
	if (Close || !Android)
		return false;

	os::Timer::tick();
	pumpEvents(false);
	postKeyboardInput();

	// Sleep while paused or without a window
	if (!Close && (Paused || Surface == EGL_NO_SURFACE))
	{
		const bool wasStopped = Timer->isStopped();
		if (!wasStopped)
			Timer->stop();
		while (!Close && (Paused || Surface == EGL_NO_SURFACE))
			pumpEvents(true);
		if (!wasStopped)
			Timer->start();
	}

	checkSurfaceSize();
	return !Close;
}

void CIrrDeviceAndroid::yield()
{
	struct timespec ts = { 0, 1 };
	nanosleep(&ts, 0);
}

void CIrrDeviceAndroid::sleep(u32 timeMs, bool pauseTimer)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t)(timeMs / 1000);
	ts.tv_nsec = (long)(timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, 0);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceAndroid::setWindowCaption(const wchar_t* text) {}

bool CIrrDeviceAndroid::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return false;
}

bool CIrrDeviceAndroid::isWindowActive() const
{
	return Focused && !Paused && Surface != EGL_NO_SURFACE;
}

bool CIrrDeviceAndroid::isWindowFocused() const
{
	return Focused;
}

bool CIrrDeviceAndroid::isWindowMinimized() const
{
	return Paused || Surface == EGL_NO_SURFACE;
}

void CIrrDeviceAndroid::closeDevice()
{
	Close = true;
}

void CIrrDeviceAndroid::setResizable(bool resize) {}
void CIrrDeviceAndroid::minimizeWindow() {}
void CIrrDeviceAndroid::maximizeWindow() {}
void CIrrDeviceAndroid::restoreWindow() {}

E_DEVICE_TYPE CIrrDeviceAndroid::getType() const
{
	return EIDT_ANDROID;
}

void CIrrDeviceAndroid::createDriver()
{
	switch (CreationParams.DriverType)
	{
	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;
	default:
		if (CreationParams.DriverType != video::EDT_OPENGL)
			os::Printer::log("Only the OpenGL driver is available on Android; using it instead of the requested driver.", ELL_INFORMATION);
		CreationParams.DriverType = video::EDT_OPENGL;
		#ifdef _IRR_COMPILE_WITH_OPENGL_
		VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, this);
		#else
		os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
		#endif
		break;
	}
}

void CIrrDeviceAndroid::handleAndroidCommand(android_app* app, int32_t cmd)
{
	CIrrDeviceAndroid* device = (CIrrDeviceAndroid*)app->userData;
	if (!device)
		return;

	switch (cmd)
	{
	case APP_CMD_INIT_WINDOW:
		if (device->Context != EGL_NO_CONTEXT)
			device->createEGLSurface();
		break;
	case APP_CMD_TERM_WINDOW:
		device->destroyEGLSurface();
		break;
	case APP_CMD_GAINED_FOCUS:
		device->Focused = true;
		break;
	case APP_CMD_LOST_FOCUS:
		device->Focused = false;
		break;
	case APP_CMD_PAUSE:
		device->Paused = true;
		if (PauseListener) PauseListener(true);
		break;
	case APP_CMD_RESUME:
		device->Paused = false;
		if (PauseListener) PauseListener(false);
		break;
	case APP_CMD_DESTROY:
		device->Close = true;
		break;
	case APP_CMD_LOW_MEMORY:
		os::Printer::log("Android reports low memory.", ELL_WARNING);
		break;
	default:
		break;
	}
}

s32 CIrrDeviceAndroid::handleInput(android_app* app, AInputEvent* event)
{
	CIrrDeviceAndroid* device = (CIrrDeviceAndroid*)app->userData;
	if (!device)
		return 0;

	switch (AInputEvent_getType(event))
	{
	case AINPUT_EVENT_TYPE_MOTION:
		return device->handleMotion(event);
	case AINPUT_EVENT_TYPE_KEY:
		return device->handleKey(event);
	default:
		return 0;
	}
}

void CIrrDeviceAndroid::CCursorControl::setPosition(f32 x, f32 y)
{
	const core::dimension2du size = Device->getSurfaceSize();
	setPosition((s32)(x * size.Width), (s32)(y * size.Height));
}

core::position2d<f32> CIrrDeviceAndroid::CCursorControl::getRelativePosition()
{
	const core::dimension2du size = Device->getSurfaceSize();
	if (size.Width == 0 || size.Height == 0)
		return core::position2d<f32>(0.f, 0.f);
	return core::position2d<f32>(CursorPos.X / (f32)size.Width, CursorPos.Y / (f32)size.Height);
}

void CIrrDeviceAndroid::postMouse(EMOUSE_INPUT_EVENT type, s32 x, s32 y, f32 wheel)
{
	static_cast<CCursorControl*>(CursorControl)->CursorPos = core::position2d<s32>(x, y);

	SEvent event;
	event.EventType = EET_MOUSE_INPUT_EVENT;
	event.MouseInput.Event = type;
	event.MouseInput.X = x;
	event.MouseInput.Y = y;
	event.MouseInput.Wheel = wheel;
	event.MouseInput.Shift = false;
	event.MouseInput.Control = false;
	event.MouseInput.ButtonStates = MouseButtons;
	postEventFromUser(event);

	if (type == EMIE_LMOUSE_PRESSED_DOWN || type == EMIE_RMOUSE_PRESSED_DOWN || type == EMIE_MMOUSE_PRESSED_DOWN)
	{
		const u32 clicks = checkSuccessiveClicks(x, y, type);
		if (clicks == 2 || clicks == 3)
		{
			event.MouseInput.Event = (EMOUSE_INPUT_EVENT)((clicks == 2 ? EMIE_LMOUSE_DOUBLE_CLICK : EMIE_LMOUSE_TRIPLE_CLICK) + type - EMIE_LMOUSE_PRESSED_DOWN);
			postEventFromUser(event);
		}
	}
}

s32 CIrrDeviceAndroid::handleMotion(AInputEvent* event)
{
	const s32 source = AInputEvent_getSource(event);
	if ((source & AINPUT_SOURCE_CLASS_JOYSTICK) != 0)
		return handleGamepadMotion(event);

	const s32 action = AMotionEvent_getAction(event);
	const s32 masked = action & AMOTION_EVENT_ACTION_MASK;
	const s32 index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

	// Real mouse
	if ((source & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE)
	{
		const s32 x = (s32)AMotionEvent_getX(event, 0);
		const s32 y = (s32)AMotionEvent_getY(event, 0);

		if (masked == AMOTION_EVENT_ACTION_SCROLL)
		{
			postMouse(EMIE_MOUSE_WHEEL, x, y, AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0));
			return 1;
		}

		postMouse(EMIE_MOUSE_MOVED, x, y);

		u32 buttons = 0;
		const s32 state = AMotionEvent_getButtonState(event);
		if (state & AMOTION_EVENT_BUTTON_PRIMARY) buttons |= EMBSM_LEFT;
		if (state & AMOTION_EVENT_BUTTON_SECONDARY) buttons |= EMBSM_RIGHT;
		if (state & AMOTION_EVENT_BUTTON_TERTIARY) buttons |= EMBSM_MIDDLE;
		// Some mice report no button state
		if (state == 0 && (masked == AMOTION_EVENT_ACTION_DOWN || masked == AMOTION_EVENT_ACTION_MOVE))
			buttons = MouseButtons & EMBSM_LEFT ? MouseButtons : (masked == AMOTION_EVENT_ACTION_DOWN ? EMBSM_LEFT : 0);

		const u32 changed = buttons ^ MouseButtons;
		MouseButtons = buttons;
		if (changed & EMBSM_LEFT) postMouse((buttons & EMBSM_LEFT) ? EMIE_LMOUSE_PRESSED_DOWN : EMIE_LMOUSE_LEFT_UP, x, y);
		if (changed & EMBSM_RIGHT) postMouse((buttons & EMBSM_RIGHT) ? EMIE_RMOUSE_PRESSED_DOWN : EMIE_RMOUSE_LEFT_UP, x, y);
		if (changed & EMBSM_MIDDLE) postMouse((buttons & EMBSM_MIDDLE) ? EMIE_MMOUSE_PRESSED_DOWN : EMIE_MMOUSE_LEFT_UP, x, y);
		return 1;
	}

	// Touch: first finger = left button, second finger = right button
	switch (masked)
	{
	case AMOTION_EVENT_ACTION_DOWN:
	{
		MousePointerId = AMotionEvent_getPointerId(event, 0);
		const s32 x = (s32)AMotionEvent_getX(event, 0);
		const s32 y = (s32)AMotionEvent_getY(event, 0);
		postMouse(EMIE_MOUSE_MOVED, x, y);
		MouseButtons |= EMBSM_LEFT;
		postMouse(EMIE_LMOUSE_PRESSED_DOWN, x, y);
		break;
	}
	case AMOTION_EVENT_ACTION_POINTER_DOWN:
		if (SecondPointerId < 0 && MousePointerId >= 0)
		{
			SecondPointerId = AMotionEvent_getPointerId(event, index);
			const core::position2di pos = CursorControl->getPosition();
			MouseButtons |= EMBSM_RIGHT;
			postMouse(EMIE_RMOUSE_PRESSED_DOWN, pos.X, pos.Y);
		}
		break;
	case AMOTION_EVENT_ACTION_MOVE:
	{
		const s32 count = (s32)AMotionEvent_getPointerCount(event);
		for (s32 i = 0; i < count; ++i)
		{
			if (AMotionEvent_getPointerId(event, i) == MousePointerId)
			{
				postMouse(EMIE_MOUSE_MOVED, (s32)AMotionEvent_getX(event, i), (s32)AMotionEvent_getY(event, i));
				break;
			}
		}
		break;
	}
	case AMOTION_EVENT_ACTION_POINTER_UP:
	{
		const s32 id = AMotionEvent_getPointerId(event, index);
		const core::position2di pos = CursorControl->getPosition();
		if (id == SecondPointerId)
		{
			SecondPointerId = -1;
			MouseButtons &= ~EMBSM_RIGHT;
			postMouse(EMIE_RMOUSE_LEFT_UP, pos.X, pos.Y);
		}
		else if (id == MousePointerId)
		{
			MousePointerId = -1;
			MouseButtons &= ~EMBSM_LEFT;
			postMouse(EMIE_LMOUSE_LEFT_UP, pos.X, pos.Y);
		}
		break;
	}
	case AMOTION_EVENT_ACTION_UP:
	case AMOTION_EVENT_ACTION_CANCEL:
	{
		const s32 x = (s32)AMotionEvent_getX(event, 0);
		const s32 y = (s32)AMotionEvent_getY(event, 0);
		if (SecondPointerId >= 0)
		{
			SecondPointerId = -1;
			MouseButtons &= ~EMBSM_RIGHT;
			postMouse(EMIE_RMOUSE_LEFT_UP, x, y);
		}
		if (MousePointerId >= 0)
		{
			MousePointerId = -1;
			MouseButtons &= ~EMBSM_LEFT;
			postMouse(EMIE_LMOUSE_LEFT_UP, x, y);
		}
		break;
	}
	default:
		return 0;
	}
	return 1;
}

s32 CIrrDeviceAndroid::gamepadSlot(s32 deviceId)
{
	for (s32 i = 0; i < MaxGamepads; ++i)
		if (Gamepads[i].DeviceId == deviceId)
			return i;
	for (s32 i = 0; i < MaxGamepads; ++i)
	{
		if (Gamepads[i].DeviceId < 0)
		{
			Gamepads[i].DeviceId = deviceId;
			return i;
		}
	}
	return -1;
}

void CIrrDeviceAndroid::postGamepad(s32 slot)
{
	SEvent event;
	event.EventType = EET_JOYSTICK_INPUT_EVENT;
	event.JoystickEvent.Joystick = (u8)slot;
	event.JoystickEvent.ButtonStates = Gamepads[slot].Buttons;
	event.JoystickEvent.POV = 65535;
	for (s32 a = 0; a < SEvent::SJoystickEvent::NUMBER_OF_AXES; ++a)
		event.JoystickEvent.Axis[a] = Gamepads[slot].Axis[a];
	postEventFromUser(event);
}

s32 CIrrDeviceAndroid::handleGamepadMotion(AInputEvent* event)
{
	const s32 slot = gamepadSlot(AInputEvent_getDeviceId(event));
	if (slot < 0)
		return 0;

	SGamepad& pad = Gamepads[slot];
	#define AXIS(a) AMotionEvent_getAxisValue(event, a, 0)
	f32 rx = AXIS(AMOTION_EVENT_AXIS_Z), ry = AXIS(AMOTION_EVENT_AXIS_RZ);
	if (rx == 0.f && ry == 0.f)
	{
		rx = AXIS(AMOTION_EVENT_AXIS_RX);
		ry = AXIS(AMOTION_EVENT_AXIS_RY);
	}
	f32 lt = AXIS(AMOTION_EVENT_AXIS_LTRIGGER), rt = AXIS(AMOTION_EVENT_AXIS_RTRIGGER);
	if (lt == 0.f) lt = AXIS(AMOTION_EVENT_AXIS_BRAKE);
	if (rt == 0.f) rt = AXIS(AMOTION_EVENT_AXIS_GAS);

	// XInput order, stick up is positive
	pad.Axis[0] = axisToS16(AXIS(AMOTION_EVENT_AXIS_X));
	pad.Axis[1] = axisToS16(-AXIS(AMOTION_EVENT_AXIS_Y));
	pad.Axis[2] = axisToS16(rx);
	pad.Axis[3] = axisToS16(-ry);
	pad.Axis[4] = axisToS16(lt);
	pad.Axis[5] = axisToS16(rt);

	// Many pads send the d-pad as a hat axis
	const f32 hatX = AXIS(AMOTION_EVENT_AXIS_HAT_X), hatY = AXIS(AMOTION_EVENT_AXIS_HAT_Y);
	#undef AXIS
	const u32 dpadMask = (1u << PAD_DPAD_UP) | (1u << PAD_DPAD_RIGHT) | (1u << PAD_DPAD_DOWN) | (1u << PAD_DPAD_LEFT);
	u32 dpad = 0;
	if (hatY < -0.5f) dpad |= 1u << PAD_DPAD_UP;
	if (hatY > 0.5f) dpad |= 1u << PAD_DPAD_DOWN;
	if (hatX < -0.5f) dpad |= 1u << PAD_DPAD_LEFT;
	if (hatX > 0.5f) dpad |= 1u << PAD_DPAD_RIGHT;
	if (hatX != 0.f || hatY != 0.f || (pad.Buttons & dpadMask))
		pad.Buttons = (pad.Buttons & ~dpadMask) | dpad;

	postGamepad(slot);
	return 1;
}

s32 CIrrDeviceAndroid::handleKey(AInputEvent* event)
{
	const s32 keyCode = AKeyEvent_getKeyCode(event);
	const s32 action = AKeyEvent_getAction(event);
	const s32 metaState = AKeyEvent_getMetaState(event);

	if (action != AKEY_EVENT_ACTION_DOWN && action != AKEY_EVENT_ACTION_UP)
		return 0;

	// Let the system handle volume keys.
	if (keyCode == AKEYCODE_VOLUME_UP || keyCode == AKEYCODE_VOLUME_DOWN || keyCode == AKEYCODE_VOLUME_MUTE)
		return 0;

	if (isFromGamepad(event))
	{
		s32 bit = gamepadButtonForKey(keyCode);
		if (bit < 0)
			bit = dpadButtonForKey(keyCode);
		if (bit >= 0)
		{
			const s32 slot = gamepadSlot(AInputEvent_getDeviceId(event));
			if (slot < 0)
				return 0;
			if (action == AKEY_EVENT_ACTION_DOWN)
				Gamepads[slot].Buttons |= 1u << bit;
			else
				Gamepads[slot].Buttons &= ~(1u << bit);
			postGamepad(slot);
			return 1;
		}
	}

	SEvent keyEvent;
	keyEvent.EventType = EET_KEY_INPUT_EVENT;
	keyEvent.KeyInput.PressedDown = action == AKEY_EVENT_ACTION_DOWN;
	keyEvent.KeyInput.Shift = (metaState & AMETA_SHIFT_ON) != 0;
	keyEvent.KeyInput.Control = (metaState & AMETA_CTRL_ON) != 0;
	keyEvent.KeyInput.Key = (keyCode >= 0 && (u32)keyCode < KeyMap.size()) ? KeyMap[keyCode] : (EKEY_CODE)0;
	keyEvent.KeyInput.Char = 0;

	// Back acts as Escape
	if (keyCode == AKEYCODE_BACK)
		keyEvent.KeyInput.Key = KEY_ESCAPE;
	else if (keyEvent.KeyInput.Key != KEY_BACK && keyEvent.KeyInput.Key != KEY_DELETE && keyEvent.KeyInput.Key != KEY_RETURN)
		keyEvent.KeyInput.Char = getUnicodeChar(action, keyCode, metaState);

	postEventFromUser(keyEvent);
	return 1;
}

// Text from the on-screen keyboard (LimeActivity)
void CIrrDeviceAndroid::postKeyboardInput()
{
	std::vector<LimeAndroid::KeyboardInput> typed;
	LimeAndroid::takeKeyboardInput(typed);

	SEvent keyEvent;
	keyEvent.EventType = EET_KEY_INPUT_EVENT;
	keyEvent.KeyInput.Shift = false;
	keyEvent.KeyInput.Control = false;
	for (u32 i = 0; i < typed.size(); ++i)
	{
		const LimeAndroid::KeyboardInput& in = typed[i];
		if (in.keyCode)
		{
			keyEvent.KeyInput.Key = (in.keyCode > 0 && (u32)in.keyCode < KeyMap.size()) ? KeyMap[in.keyCode] : (EKEY_CODE)0;
			keyEvent.KeyInput.Char = 0;
			keyEvent.KeyInput.PressedDown = true;
			postEventFromUser(keyEvent);
			keyEvent.KeyInput.PressedDown = false;
			postEventFromUser(keyEvent);
			continue;
		}
		for (size_t c = 0; c < in.text.size(); ++c)
		{
			keyEvent.KeyInput.Key = (EKEY_CODE)0;
			keyEvent.KeyInput.Char = (wchar_t)in.text[c];
			keyEvent.KeyInput.PressedDown = true;
			postEventFromUser(keyEvent);
			keyEvent.KeyInput.PressedDown = false;
			postEventFromUser(keyEvent);
		}
	}
}

wchar_t CIrrDeviceAndroid::getUnicodeChar(s32 action, s32 keyCode, s32 metaState)
{
	if (!Android || !Android->activity || !Android->activity->vm)
		return 0;

	if (!JNIEnvAttached)
	{
		JavaVMAttachArgs args = { JNI_VERSION_1_6, "LimeMain", 0 };
		if (Android->activity->vm->AttachCurrentThread(&JNIEnvAttached, &args) != JNI_OK)
		{
			JNIEnvAttached = 0;
			return 0;
		}
	}

	JNIEnv* env = JNIEnvAttached;
	if (!KeyEventClass)
	{
		jclass local = env->FindClass("android/view/KeyEvent");
		if (!local)
		{
			env->ExceptionClear();
			return 0;
		}
		KeyEventClass = (jclass)env->NewGlobalRef(local);
		env->DeleteLocalRef(local);
		KeyEventCtor = env->GetMethodID(KeyEventClass, "<init>", "(II)V");
		KeyEventGetUnicodeChar = env->GetMethodID(KeyEventClass, "getUnicodeChar", "(I)I");
	}
	if (!KeyEventCtor || !KeyEventGetUnicodeChar)
		return 0;

	jobject keyEvent = env->NewObject(KeyEventClass, KeyEventCtor, action, keyCode);
	jint ch = keyEvent ? env->CallIntMethod(keyEvent, KeyEventGetUnicodeChar, metaState) : 0;
	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		ch = 0;
	}
	if (keyEvent)
		env->DeleteLocalRef(keyEvent);
	return (wchar_t)ch;
}

void CIrrDeviceAndroid::createKeyMap()
{
	KeyMap.set_used(AKEYCODE_NUMPAD_RIGHT_PAREN + 1);
	for (u32 i = 0; i < KeyMap.size(); ++i)
		KeyMap[i] = (EKEY_CODE)0;

	KeyMap[AKEYCODE_HOME] = KEY_HOME;
	KeyMap[AKEYCODE_BACK] = KEY_ESCAPE;
	for (s32 i = 0; i <= 9; ++i)
		KeyMap[AKEYCODE_0 + i] = (EKEY_CODE)(KEY_KEY_0 + i);
	for (s32 i = 0; i < 26; ++i)
		KeyMap[AKEYCODE_A + i] = (EKEY_CODE)(KEY_KEY_A + i);
	KeyMap[AKEYCODE_DPAD_UP] = KEY_UP;
	KeyMap[AKEYCODE_DPAD_DOWN] = KEY_DOWN;
	KeyMap[AKEYCODE_DPAD_LEFT] = KEY_LEFT;
	KeyMap[AKEYCODE_DPAD_RIGHT] = KEY_RIGHT;
	KeyMap[AKEYCODE_DPAD_CENTER] = KEY_RETURN;
	KeyMap[AKEYCODE_CLEAR] = KEY_CLEAR;
	KeyMap[AKEYCODE_COMMA] = KEY_COMMA;
	KeyMap[AKEYCODE_PERIOD] = KEY_PERIOD;
	KeyMap[AKEYCODE_ALT_LEFT] = KEY_LMENU;
	KeyMap[AKEYCODE_ALT_RIGHT] = KEY_RMENU;
	KeyMap[AKEYCODE_SHIFT_LEFT] = KEY_LSHIFT;
	KeyMap[AKEYCODE_SHIFT_RIGHT] = KEY_RSHIFT;
	KeyMap[AKEYCODE_TAB] = KEY_TAB;
	KeyMap[AKEYCODE_SPACE] = KEY_SPACE;
	KeyMap[AKEYCODE_ENTER] = KEY_RETURN;
	KeyMap[AKEYCODE_DEL] = KEY_BACK;
	KeyMap[AKEYCODE_GRAVE] = KEY_OEM_3;
	KeyMap[AKEYCODE_MINUS] = KEY_MINUS;
	KeyMap[AKEYCODE_EQUALS] = KEY_PLUS;
	KeyMap[AKEYCODE_LEFT_BRACKET] = KEY_OEM_4;
	KeyMap[AKEYCODE_RIGHT_BRACKET] = KEY_OEM_6;
	KeyMap[AKEYCODE_BACKSLASH] = KEY_OEM_5;
	KeyMap[AKEYCODE_SEMICOLON] = KEY_OEM_1;
	KeyMap[AKEYCODE_APOSTROPHE] = KEY_OEM_7;
	KeyMap[AKEYCODE_SLASH] = KEY_OEM_2;
	KeyMap[AKEYCODE_PLUS] = KEY_PLUS;
	KeyMap[AKEYCODE_MENU] = KEY_APPS;
	KeyMap[AKEYCODE_PAGE_UP] = KEY_PRIOR;
	KeyMap[AKEYCODE_PAGE_DOWN] = KEY_NEXT;
	KeyMap[AKEYCODE_ESCAPE] = KEY_ESCAPE;
	KeyMap[AKEYCODE_FORWARD_DEL] = KEY_DELETE;
	KeyMap[AKEYCODE_CTRL_LEFT] = KEY_LCONTROL;
	KeyMap[AKEYCODE_CTRL_RIGHT] = KEY_RCONTROL;
	KeyMap[AKEYCODE_CAPS_LOCK] = KEY_CAPITAL;
	KeyMap[AKEYCODE_SCROLL_LOCK] = KEY_SCROLL;
	KeyMap[AKEYCODE_META_LEFT] = KEY_LWIN;
	KeyMap[AKEYCODE_META_RIGHT] = KEY_RWIN;
	KeyMap[AKEYCODE_SYSRQ] = KEY_SNAPSHOT;
	KeyMap[AKEYCODE_BREAK] = KEY_PAUSE;
	KeyMap[AKEYCODE_MOVE_HOME] = KEY_HOME;
	KeyMap[AKEYCODE_MOVE_END] = KEY_END;
	KeyMap[AKEYCODE_INSERT] = KEY_INSERT;
	for (s32 i = 0; i < 12; ++i)
		KeyMap[AKEYCODE_F1 + i] = (EKEY_CODE)(KEY_F1 + i);
	KeyMap[AKEYCODE_NUM_LOCK] = KEY_NUMLOCK;
	for (s32 i = 0; i <= 9; ++i)
		KeyMap[AKEYCODE_NUMPAD_0 + i] = (EKEY_CODE)(KEY_NUMPAD0 + i);
	KeyMap[AKEYCODE_NUMPAD_DIVIDE] = KEY_DIVIDE;
	KeyMap[AKEYCODE_NUMPAD_MULTIPLY] = KEY_MULTIPLY;
	KeyMap[AKEYCODE_NUMPAD_SUBTRACT] = KEY_SUBTRACT;
	KeyMap[AKEYCODE_NUMPAD_ADD] = KEY_ADD;
	KeyMap[AKEYCODE_NUMPAD_DOT] = KEY_DECIMAL;
	KeyMap[AKEYCODE_NUMPAD_COMMA] = KEY_SEPARATOR;
	KeyMap[AKEYCODE_NUMPAD_ENTER] = KEY_RETURN;
}

} // end namespace irr

// LimeAndroid.h
static irr::CIrrDeviceAndroid* asAndroidDevice(irr::IrrlichtDevice* device)
{
	return (device && device->getType() == irr::EIDT_ANDROID) ? static_cast<irr::CIrrDeviceAndroid*>(device) : 0;
}

bool LimeAndroid::getSurfaceSize(irr::IrrlichtDevice* device, int* width, int* height)
{
	irr::CIrrDeviceAndroid* android = asAndroidDevice(device);
	if (!android)
		return false;
	const irr::core::dimension2du size = android->getSurfaceSize();
	*width = (int)size.Width;
	*height = (int)size.Height;
	return size.Width > 0 && size.Height > 0;
}

void LimeAndroid::setSwapInterval(irr::IrrlichtDevice* device, int interval)
{
	if (irr::CIrrDeviceAndroid* android = asAndroidDevice(device))
		android->setSwapInterval(interval);
}

void LimeAndroid::setPauseListener(void (*listener)(bool paused))
{
	PauseListener = listener;
}

#endif // _IRR_COMPILE_WITH_ANDROID_DEVICE_
