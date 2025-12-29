#pragma once
#include <string>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class Window {
public:
	Window();
	~Window();

	bool Create(int w, int h, std::string title, bool fullscreen, bool resizable, bool maintainAR);
	void Close();
	void PollEvents();
	bool ShouldClose();
	bool SetIcon(std::string path);
	void SetTitle(std::string path);

	HWND GetHandle() const { return glfwGetWin32Window(glfwWindow); }
	GLFWwindow* getGLFWWindow() const { return glfwWindow; }
private:
	GLFWwindow* glfwWindow = nullptr;
};