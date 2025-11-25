#pragma once
#include <irrlicht.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class DebugConsole;
class Window;

class Renderer {
public:
	Renderer();
	~Renderer();

	bool Create(int driver, int w, int h, bool stencil);
	void Close();
	void SetDebugConsole(DebugConsole* d);
	void SetWindow(Window* w);
	HWND GetHandle();
	bool Render();
private:
	DebugConsole* console = nullptr;
	Window* window = nullptr;

	irr::IrrlichtDevice* device = nullptr;
	irr::video::IVideoDriver* driver = nullptr;
	irr::scene::ISceneManager* smgr = nullptr;

	// Generic Scene
	irr::video::SColor bgColor = irr::video::SColor(255, 255, 0, 0);
	irr::scene::ISceneNode* skydome = nullptr;

	// Active Camera
	irr::scene::ICameraSceneNode* activeCamera = nullptr;
	irr::scene::ISceneNode* activeCameraForward = nullptr;
};