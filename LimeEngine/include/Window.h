#pragma once
#include <string>
#include <memory>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>

class Application;

class Vec2;
class Vec4;
class Texture;
class Event;

class Window {
public:
	Window(Application* app);
	~Window();

	bool Create();
	void Close();
	void PollEvents();
	bool ShouldClose();
	void PreUpdateBG();
	void EndFrame();
	HWND GetHandle() const { return glfwGetWin32Window(glfwWindow); }
	GLFWwindow* getGLFWWindow() const { return glfwWindow; }
	void Focus();
	bool guardEditCheck();

	void setMouseType(int i);
	int getMouseType();

	void setTitle(std::string path);
	void doFullscreen(bool v);
	Vec2 getPosition();
	void setPosition(const Vec2& pos);
	Vec2 getSize();
	void setSize(const Vec2& size);
	void setSizeSimple(int w, int h) { windowSize.x = w; windowSize.y = h; }
	Vec2 getMonitorSize();
	bool isFocused();
	bool getResizable() { return resizable; }
	void setResizable(bool on);
	void keepAspectRatio(bool on);
	void setSizeLimit(int w, int h);
	// void setMouseLocked(bool v);
	void syncMouse(double* mx, double* my);
	void setMinimumSize(const Vec2& size);

	// To get the raw window size
	Vec2 getRawWinSize() const;
	float getWinAR() const;

	int getTime();
	void setSwapInterval(int i);
	int getPrimaryHz();

	std::shared_ptr<Event> WindowResize = nullptr;
private:
	struct Vec2S { float x, y; };

	bool isFullscreened = false;
	GLFWwindow* glfwWindow = nullptr;
	Vec2S preFullWinSize{};
	Vec2S preFullWinPos{};
	Vec2S windowSize{};
	bool resizable = true;
	bool inFullscreenCallback = false;
	bool didCallback = false; // Used when the maximize/restore callback was called, resets before polling events

	int mouseType = 0;

	GLFWmonitor* getCurrentMonitor();
};