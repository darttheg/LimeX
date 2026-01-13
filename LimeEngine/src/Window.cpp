#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Window.h"

Window::Window() {}

Window::~Window() {
}

void Window::Close() {
	if (glfwWindow)
		glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

bool Window::Create(int w, int h, std::string title, bool fullscreen, bool resizable, bool maintainAR) {
	if (!glfwInit()) return false;

	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, !fullscreen ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindow = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
	if (!glfwWindow) return false;

	if (!fullscreen) {
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		int x = (mode->width - w) / 2;
		int y = (mode->height - h) / 2;
		glfwSetWindowPos(glfwWindow, x, y);
	}

	if (maintainAR)
		glfwSetWindowAspectRatio(glfwWindow, w, h);
	else
		glfwSetWindowAspectRatio(glfwWindow, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// TODO: Set maximize/restore callbacks
	// TODO: Set framebuffer size callback (updateIrrRenderRes...)

	return true;
}

void Window::PollEvents() {
	glfwPollEvents();
	glfwMakeContextCurrent(glfwWindow);
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(glfwWindow);
}

bool Window::SetIcon(std::string path) {
	if (!glfwWindow) return false;

	GLFWimage images[1];
	images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
	if (images[0].pixels) {
		glfwSetWindowIcon(glfwWindow, 1, images);
		stbi_image_free(images[0].pixels);
	}
	else { return false; }

	return true;
}

void Window::SetTitle(std::string path) {
	if (!glfwWindow) return;

	glfwSetWindowTitle(glfwWindow, path.c_str());
}