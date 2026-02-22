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

	void setGLFWCallbackTriggered(bool v) { maximizeRestoreCBThisFrame = v; }
	bool getGLFWCallbackTriggered() { return maximizeRestoreCBThisFrame; }
	void setViewport(int x, int y, int w, int h);
	int getTime();
	void setSwapInterval(int i);
	int getPrimaryHz();

	std::shared_ptr<Event> WindowResize = nullptr;
private:
	struct Vec2S { float x, y; };

	bool maximizeRestoreCBThisFrame = false; // Used to ignore frame buffer resize callback
	bool isFullscreened = false;
	GLFWwindow* glfwWindow = nullptr;
	Vec2S windowSize{};
	bool resizable = true;
};