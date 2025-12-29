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

	int GetFrameRate();
	float GetDeltaTime();
	void SetFrameLimit(int v);
	void SetVSync(bool v);
	int GetElapsedTime();

private:
	DebugConsole* console = nullptr;
	Window* window = nullptr;

	std::unique_ptr<Ogre::Root> o_Root;
	Ogre::RenderWindow* o_Window = nullptr;
	Ogre::SceneManager* o_SceneManager = nullptr;
	bool isCreated = false;

	// Ogre Frame Hook
	float deltaTime = 0.0f;
	struct FrameHook;
	std::unique_ptr<FrameHook> hook;
};