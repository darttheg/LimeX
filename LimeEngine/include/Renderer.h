#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>
#include <memory>

#include "irrlicht.h"
using namespace irr;

class DebugConsole;
class Window;

class Renderer {
public:
	Renderer();
	~Renderer();

	bool Init(DebugConsole* d, Window* w);
	bool Shutdown();
	bool Render();
private:
	DebugConsole* console = nullptr;
	Window* window = nullptr;

	bool isCreated = false;

	// Renderer parameters
	bool doMatchResolution = true; // Hook resolution w/h to window size

	// Irrlicht
	irr::IrrlichtDevice* i_device = nullptr;
	irr::scene::ISceneManager* i_smgr = nullptr;
	irr::video::IVideoDriver* i_driver = nullptr;
	irr::gui::IGUIEnvironment* i_gui = nullptr;
	irr::video::IGPUProgrammingServices* i_gpu = nullptr;
};