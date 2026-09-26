#ifdef __ANDROID__
#include "Window.h"

#include "Objects/Vec2.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "Receiver.h"

#include "Objects/Event.h"

#include "irrlicht.h"
#include "LimeAndroid.h"
#include <android_native_app_glue.h>
#include <chrono>

static Application* a;
static DebugConsole* d;
static std::chrono::steady_clock::time_point startTime;

Window::Window(Application* app) {
	a = app;
	d = a->GetDebugConsole();
}

Window::~Window() {}

irr::IrrlichtDevice* Window::getDevice() const {
	return a->GetRenderer() ? a->GetRenderer()->getDevice() : nullptr;
}

void Window::Close() {
	created = false;
}

bool Window::Create() {
	android_app* app = LimeAndroid::getApp();
	if (!app || !app->window) return false;

	windowSize.x = (float)ANativeWindow_getWidth(app->window);
	windowSize.y = (float)ANativeWindow_getHeight(app->window);
	preFullWinSize = windowSize;
	isFullscreened = true;
	resizable = false;

	startTime = std::chrono::steady_clock::now();
	created = true;
	return true;
}

void Window::PollEvents() {
	didCallback = false;
	irr::IrrlichtDevice* device = getDevice();
	if (!device) return;

	if (!device->run()) {
		closeRequested = true;
		return;
	}

	int width = 0;
	int height = 0;
	if (LimeAndroid::getSurfaceSize(device, &width, &height) && (width != (int)windowSize.x || height != (int)windowSize.y)) {
		auto* r = a->GetRenderer();
		a->GetReceiver()->setSkipDelta();
		setSizeSimple(width, height);
		r->updateWindowSize(width, height);
		if (r->getMatchRes() && WindowResize)
			WindowResize.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); });
	}
}

bool Window::ShouldClose() {
	return closeRequested;
}

void Window::PreUpdateBG() {} // Desktop only

void Window::EndFrame() {
	inFullscreenCallback = false;
}

void Window::Focus() { guardEditCheck(); } // Desktop only

bool Window::guardEditCheck() {
	if (!created) {
		std::string out = "The window cannot be modified until it has been created.";
		d->PostError(out, true, true);
		return false;
	}
	return true;
}

void Window::setMouseType(int i) {
	if (!guardEditCheck()) return;
	mouseType = i;
}

int Window::getMouseType() {
	if (!guardEditCheck()) return 0;
	return mouseType;
}

void Window::syncMouse(double* mx, double* my) {
	irr::IrrlichtDevice* device = getDevice();
	if (!device || !device->getCursorControl()) return;
	const irr::core::position2di pos = device->getCursorControl()->getPosition();
	*mx = pos.X;
	*my = pos.Y;
}

void Window::setMinimumSize(const Vec2& size) { guardEditCheck(); } // Desktop only

void Window::setTitle(std::string path) { guardEditCheck(); } // Desktop only

void Window::doFullscreen(bool v) { guardEditCheck(); } // Desktop only

Vec2 Window::getPosition() { guardEditCheck(); return Vec2(); } // Desktop only

void Window::setPosition(const Vec2& pos) { guardEditCheck(); } // Desktop only

Vec2 Window::getSize() {
	if (!guardEditCheck()) return Vec2();
	return Vec2(windowSize.x, windowSize.y);
}

void Window::setSize(const Vec2& size) { guardEditCheck(); } // Desktop only

Vec2 Window::getRenderedSize() {
	if (!guardEditCheck()) return Vec2();
	bool s = a->GetRenderer()->getMatchRes();
	return Vec2(s ? windowSize.x : a->GetRenderer()->getRenderSize().getX(),
				s ? windowSize.y : a->GetRenderer()->getRenderSize().getY());
}

Vec2 Window::getMonitorSize() {
	if (!guardEditCheck()) return Vec2();
	return Vec2(windowSize.x, windowSize.y);
}

bool Window::isFocused() {
	if (!guardEditCheck()) return false;
	irr::IrrlichtDevice* device = getDevice();
	return device ? device->isWindowFocused() : true;
}

void Window::setResizable(bool on) {
	WindowConfig c = a->GetConfig();
	c.resizable = on;
	a->SetConfig(c);
}

void Window::keepAspectRatio(bool on) { guardEditCheck(); } // Desktop only

void Window::setSizeLimit(int w, int h) { guardEditCheck(); } // Desktop only

Vec2 Window::getRawWinSize() const {
	return Vec2(windowSize.x, windowSize.y);
}

float Window::getWinAR() const {
	return windowSize.y > 0 ? windowSize.x / windowSize.y : 1.0f;
}

int Window::getTime() {
	return (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
}

void Window::setSwapInterval(int i) {
	LimeAndroid::setSwapInterval(getDevice(), i);
}

int Window::getPrimaryHz() {
	return 60;
}

#endif