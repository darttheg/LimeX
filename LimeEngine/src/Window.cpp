#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Window.h"
#include <GL/GL.h>

#include "Objects/Vec2.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"
#include "Receiver.h"

#include "Objects/Event.h"

static Application* a;
static DebugConsole* d;
static Window* w;

static void SetMainWindowIcon(HWND hwnd) {
	HICON hBig = (HICON)LoadImageW(
		GetModuleHandleW(nullptr),
		MAKEINTRESOURCEW(1),
		IMAGE_ICON,
		32, 32,
		LR_DEFAULTCOLOR
	);

	HICON hSmall = (HICON)LoadImageW(
		GetModuleHandleW(nullptr),
		MAKEINTRESOURCEW(1),
		IMAGE_ICON,
		16, 16,
		LR_DEFAULTCOLOR
	);

	if (hBig)   SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);
	if (hSmall) SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
}

Window::Window(Application* app) {
	a = app;
	d = a->GetDebugConsole();
	w = this;
}

Window::~Window() {
}

void Window::Close() {
	if (glfwWindow)
		glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

static WINDOWPLACEMENT wpPrev{ sizeof(wpPrev) };
static LONG_PTR stylePrev = 0;
static LONG_PTR exStylePrev = 0;
bool Window::Create() {
	if (!glfwInit()) return false;

	WindowConfig cfg = a->GetConfig();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, cfg.resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, !cfg.fullscreen ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	windowSize.x = cfg.windowSize[0];
	windowSize.y = cfg.windowSize[1];
	glfwWindow = glfwCreateWindow(windowSize.x, windowSize.y, cfg.title.c_str(), nullptr, nullptr);
	if (!glfwWindow) return false;

	if (!cfg.fullscreen) {
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		int x = (mode->width - cfg.windowSize[0]) / 2;
		int y = (mode->height - cfg.windowSize[1]) / 2;
		glfwSetWindowPos(glfwWindow, x, y);
	}

	if (cfg.keepAspectRatio)
		glfwSetWindowAspectRatio(glfwWindow, cfg.windowSize[0], cfg.windowSize[1]);
	else
		glfwSetWindowAspectRatio(glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glfwSetWindowPosCallback(glfwWindow, [](GLFWwindow* win, int x, int y) {
		HWND hwnd = a->GetWindow()->GetHandle();
		auto* d = a->GetDebugConsole();
		auto* w = a->GetWindow();
		auto* r = a->GetRenderer();

		if (w->inFullscreenCallback) return;

		w->preFullWinPos.x = x;
		w->preFullWinPos.y = y;
	});

	glfwSetWindowMaximizeCallback(glfwWindow, [](GLFWwindow* win, int maximized) {
		HWND hwnd = a->GetWindow()->GetHandle();
		auto* d = a->GetDebugConsole();
		auto* w = a->GetWindow();
		auto* r = a->GetRenderer();
		w->inFullscreenCallback = true;

		if (w->didCallback) return;
		w->didCallback = true;

		if (maximized) {
			glfwRestoreWindow(win);
			w->doFullscreen(true);
		} else {
			w->doFullscreen(false);
		}
	});

	glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
		auto* r = a->GetRenderer();
		auto* w = a->GetWindow();
		auto* rec = a->GetReceiver();
		rec->setSkipDelta();

		if (w->inFullscreenCallback && !w->isFullscreened) {
			goto resizeEvent;
		}

		if (w->isFullscreened) {
			GLFWmonitor* m = w->getCurrentMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(m);
			width = mode->width;
			height = mode->height;
			// d->Warn(std::to_string(width) + ", " + std::to_string(height));
		}

		w->setSizeSimple(width, height);
		r->updateWindowSize(width, height);

	resizeEvent:;
		r->setOnResize(width, height);
		if (r->getMatchRes()) w->WindowResize.get()->engineRun([&](const std::string& msg) { d->PostError(msg); });
	});

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetTime(0.0);

	#ifdef _WIN32
	SetMainWindowIcon(GetHandle());
	#endif

	// Temporary? Set window size limits to avoid weird letterboxing.
	setSizeLimit(cfg.renderSize[0], cfg.renderSize[1]);

	int x, y;
	glfwGetWindowPos(glfwWindow, &x, &y);
	preFullWinPos.x = x;
	preFullWinPos.y = y;

	glfwFocusWindow(glfwWindow);

	glfwMakeContextCurrent(glfwWindow);

	return true;
}

void Window::PollEvents() {
	didCallback = false;
	glfwPollEvents();
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(glfwWindow);
}

void Window::PreUpdateBG() {
	if (!a->GetRenderer()) return;
	auto* r = a->GetRenderer();

	// glClearColor(0.f, 0.f, 0.f, 1.f);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::EndFrame() {
	// glfwSwapBuffers(glfwWindow);
	inFullscreenCallback = false;
}

// ---

void Window::Focus() {
	if (!glfwWindow) return;
	glfwRestoreWindow(glfwWindow);
	glfwShowWindow(glfwWindow);
	glfwFocusWindow(glfwWindow);
}

bool Window::guardEditCheck() {
	if (!glfwWindow) {
		std::string out = "The window cannot be modified until it has been created.";
		d->PostError(out, true, true);
		return false;
	}
	return true;
}

void Window::setMouseLocked(bool v) {
	if (!guardEditCheck()) return;
	if (v) glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED); else glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::syncMouse(double* mx, double* my) {
	if (!glfwWindow) return;
	glfwGetCursorPos(glfwWindow, mx, my);
}

#undef min
#undef max
void Window::setMinimumSize(const Vec2& size) {
	if (!guardEditCheck()) return;

	int x = std::max((int)size.getX(), (int)a->GetRenderer()->getRenderSize().getX());
	int y = std::max((int)size.getY(), (int)a->GetRenderer()->getRenderSize().getY());

	setSizeLimit(x, y);
}

void Window::setTitle(std::string path) {
	if (!guardEditCheck()) return;
	glfwSetWindowTitle(glfwWindow, path.c_str());
}

void Window::doFullscreen(bool v) {
	if (!guardEditCheck()) return;
	
	if (v == isFullscreened) return;
	isFullscreened = v;
	inFullscreenCallback = true;

	if (v) {
		preFullWinSize.x = windowSize.x;
		preFullWinSize.y = windowSize.y;

		GLFWmonitor* m = getCurrentMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(m);
		int mx, my;
		glfwGetMonitorPos(m, &mx, &my);

		glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowPos(glfwWindow, mx, my);
		glfwSetWindowSize(glfwWindow, mode->width, mode->height);
	} else {
		glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_TRUE);
		glfwRestoreWindow(glfwWindow);
		int width = preFullWinSize.x;
		int height = preFullWinSize.y;
		setSizeSimple(width, height);
		a->GetRenderer()->updateWindowSize(width, height);
		glfwSetWindowSize(glfwWindow, width, height);

		glfwSetWindowPos(glfwWindow, preFullWinPos.x, preFullWinPos.y);
		glfwSetWindowAttrib(glfwWindow, GLFW_MAXIMIZED, GLFW_FALSE);
	}
}

Vec2 Window::getPosition() {
	if (!guardEditCheck()) return Vec2();

	int x = 0;
	int y = 0;
	glfwGetWindowPos(glfwWindow, &x, &y);
	return Vec2(x, y);
}

void Window::setPosition(const Vec2& pos) {
	if (!guardEditCheck()) return;
	glfwSetWindowPos(glfwWindow, static_cast<int>(pos.getX()), static_cast<int>(pos.getY()));
}

Vec2 Window::getSize() {
	if (!guardEditCheck()) return Vec2();
	return Vec2(windowSize.x, windowSize.y);
}

void Window::setSize(const Vec2& size) {
	if (!guardEditCheck()) return;
	if (size.getX() == windowSize.x && size.getY() == windowSize.y) return;

	windowSize.x = size.getX();
	windowSize.y = size.getY();

	int winX, winY;
	glfwGetWindowPos(glfwWindow, &winX, &winY);

	int oldW, oldH;
	glfwGetWindowSize(glfwWindow, &oldW, &oldH);
	glfwSetWindowSize(glfwWindow, static_cast<int>(windowSize.x), static_cast<int>(windowSize.y));

	int deltaW = static_cast<int>(windowSize.x) - oldW;
	int deltaH = static_cast<int>(windowSize.y) - oldH;
	glfwSetWindowPos(glfwWindow, winX - deltaW / 2, winY - deltaH / 2);

	// Skip delta on ACTUAL resize
	glfwGetWindowSize(glfwWindow, &winX, &winY);
	if ((winX != oldW) && (winY != oldH))
		a->GetReceiver()->setSkipDeltaMouse(true);
}

Vec2 Window::getMonitorSize() {
	if (!guardEditCheck()) return Vec2();

	int foo, bar, w, h;
	glfwGetMonitorWorkarea(getCurrentMonitor(), &foo, &bar, &w, &h);
	/*
	#ifdef _WIN32
	return Vec2(
		static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
		static_cast<float>(GetSystemMetrics(SM_CYSCREEN))
	);
	#endif
	*/

	return Vec2(w, h);
}

bool Window::isFocused() {
	if (!guardEditCheck()) return false;
	return GetForegroundWindow() == glfwGetWin32Window(glfwWindow);
}

void Window::setResizable(bool on) {
	WindowConfig c = a->GetConfig();
	c.resizable = on;
	a->SetConfig(c);

	if (!guardEditCheck()) return;
	glfwSetWindowAttrib(glfwWindow, GLFW_RESIZABLE, on ? GLFW_TRUE : GLFW_FALSE);
}

void Window::keepAspectRatio(bool on) {
	if (!guardEditCheck()) return;

	if (on)
		glfwSetWindowAspectRatio(glfwWindow, windowSize.x, windowSize.y);
	else
		glfwSetWindowAspectRatio(glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void Window::setSizeLimit(int w, int h) {
	if (!glfwWindow) return;

	glfwSetWindowSizeLimits(glfwWindow, w, h, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

Vec2 Window::getRawWinSize() const {
	return Vec2(windowSize.x, windowSize.y);
}

float Window::getWinAR() const {
	return (float)windowSize.x / (float)windowSize.y;
}

int Window::getTime() {
	return (int)(glfwGetTime() * 1000.0);
}

void Window::setSwapInterval(int i) {
	glfwSwapInterval(i);
}

int Window::getPrimaryHz() {
	GLFWmonitor* m = getCurrentMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(m);
	return mode ? mode->refreshRate : 0;
}

#include "External/GetCurrentMonitor.h"
GLFWmonitor* Window::getCurrentMonitor() {
	GLFWmonitor* m = nullptr;
	glfw_get_window_monitor(&m, glfwWindow);
	return m;
}
