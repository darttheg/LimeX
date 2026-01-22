#pragma once
#include <string>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>

class Application;

class Window {
public:
	Window(Application* app);
	~Window();

	bool Create();
	void Close();
	void PollEvents();
	bool ShouldClose();
	bool SetIcon(std::string path);
	void SetTitle(std::string path);

	HWND GetHandle() const { return glfwGetWin32Window(glfwWindow); }
	GLFWwindow* getGLFWWindow() const { return glfwWindow; }
private:
	GLFWwindow* glfwWindow = nullptr;

	Application* a = nullptr;
};