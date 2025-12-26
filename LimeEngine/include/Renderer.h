#pragma once

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <memory>

namespace Ogre {
	class Root;
	class RenderWindow;
	class SceneManager;
	class Camera;
	class Viewport;
}

class DebugConsole;
class Window;

class Renderer {
public:
	Renderer();
	~Renderer();

	bool Create();
	void Close();
	void SetDebugConsole(DebugConsole* d);
	void SetWindow(Window* w);
	bool Render();
private:
	DebugConsole* console = nullptr;
	Window* window = nullptr;

	std::unique_ptr<Ogre::Root> o_Root;
	Ogre::RenderWindow* o_Window = nullptr;
	Ogre::SceneManager* o_SceneManager = nullptr;
	Ogre::Camera* o_Camera = nullptr;
	Ogre::Viewport* o_Viewport = nullptr;
	bool isCreated = false;
};