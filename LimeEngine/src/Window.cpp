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

#ifdef _WIN32
#include <windows.h>

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
#endif

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

	glfwWindowHint(GLFW_RESIZABLE, cfg.resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, !cfg.fullscreen ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindow = glfwCreateWindow(cfg.windowSize[0], cfg.windowSize[1], cfg.title.c_str(), nullptr, nullptr);
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

	glfwSetWindowMaximizeCallback(glfwWindow, [](GLFWwindow* win, int maximized) {
		HWND hwnd = a->GetWindow()->GetHandle();

		if (maximized) {
			a->GetRenderer()->maximizeDevice();
			GetWindowPlacement(hwnd, &wpPrev);
			stylePrev = GetWindowLongPtr(hwnd, GWL_STYLE);
			exStylePrev = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

			SetWindowLongPtr(hwnd, GWL_STYLE, (stylePrev & ~WS_OVERLAPPEDWINDOW) | WS_POPUP | WS_VISIBLE);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, (exStylePrev & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)));

			HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi{ sizeof(mi) };
			GetMonitorInfo(mon, &mi);

			const RECT r = mi.rcMonitor;
			SetWindowPos(hwnd, HWND_TOP,
				r.left, r.top, r.right - r.left, r.bottom - r.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			a->GetRenderer()->updateRenderResolution(r.right - r.left, r.bottom - r.top);
		}
		else {
			a->GetRenderer()->restoreDevice();

			SetWindowLongPtr(hwnd, GWL_STYLE, stylePrev);
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStylePrev);
			SetWindowPlacement(hwnd, &wpPrev);

			SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}

		a->GetWindow()->setGLFWCallbackTriggered(true);
		});

	glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
		w->setSizeSimple(width, height);
		auto* r = a->GetRenderer();
		auto w = a->GetWindow();
		if (!w->getGLFWCallbackTriggered())
			r->updateRenderResolution(width, height);
		else
			w->setGLFWCallbackTriggered(false);

		w->WindowResize.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); });
	});

	glfwSetTime(0.0);

	#ifdef _WIN32
	SetMainWindowIcon(GetHandle());
	#endif

	return true;
}

void Window::PollEvents() {
	glfwMakeContextCurrent(glfwWindow);
	glfwPollEvents();
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(glfwWindow);
}

void Window::PreUpdateBG() {
	if (!a->GetRenderer()) return;
	auto* r = a->GetRenderer();
	Vec4 vp = r->getViewport();

	glViewport(0, 0, vp.getZ(), vp.getW());
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(vp.getX(), vp.getY(), vp.getZ(), vp.getW());
}

void Window::EndFrame() {
	glfwSwapBuffers(glfwWindow);
}

// ---

void Window::Focus() {
	if (!glfwWindow) return;
	glfwFocusWindow(glfwWindow);
}

bool Window::guardEditCheck() {
	if (!glfwWindow) {
		d->Warn("The window cannot be modified until it has been created!");
		return false;
	}
	return true;
}

void Window::setTitle(std::string path) {
	if (!guardEditCheck()) return;
	glfwSetWindowTitle(glfwWindow, path.c_str());
}

void Window::doFullscreen(bool v) {
	if (!guardEditCheck()) return;
	if (v) {
		glfwMaximizeWindow(glfwWindow);
	} else {
		glfwRestoreWindow(glfwWindow);
	}
	isFullscreened = v;
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

	a->GetRenderer()->updateRenderResolution(windowSize.x, windowSize.y);

	// Skip delta on ACTUAL resize
	glfwGetWindowSize(glfwWindow, &winX, &winY);
	if ((winX != oldW) && (winY != oldH))
		a->GetReceiver()->setSkipDeltaMouse(true);
}

Vec2 Window::getMonitorSize() {
	if (!guardEditCheck()) return Vec2();

#ifdef _WIN32
	return Vec2(
		static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
		static_cast<float>(GetSystemMetrics(SM_CYSCREEN))
	);
#endif

	return Vec2();
}

bool Window::isFocused() {
	if (!guardEditCheck()) return false;
	return a->GetRenderer()->isFocused();
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

void Window::setViewport(int x, int y, int w, int h) {
	glViewport(x, y, w, h);
}

int Window::getTime() {
	return (int)(glfwGetTime() * 1000.0);
}

void Window::setSwapInterval(int i) {
	glfwSwapInterval(i);
}

int Window::getPrimaryHz() {
	GLFWmonitor* m = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(m);
	return mode ? mode->refreshRate : 0;
}
