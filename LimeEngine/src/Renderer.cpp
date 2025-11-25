#include "Renderer.h"

#include "DebugConsole.h"
#include "Window.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

using namespace irr;
using namespace core;
using namespace video;

Renderer::Renderer() {}
Renderer::~Renderer() {
	if (device)
		device->drop();
}

void Renderer::Close() {
	if (device)
		device->drop();
}

void Renderer::SetDebugConsole(DebugConsole* d) {
	console = d;
}

void Renderer::SetWindow(Window* w) {
	window = w;
}

bool Renderer::Create(int vdriver, int w, int h, bool stencil) {
	if (device) {
		// Handle drop logic for restarting
	}

	SIrrlichtCreationParameters params;
	params.DriverType = (E_DRIVER_TYPE)vdriver;
	params.WindowSize = dimension2d<u32>(w, h);
	params.Bits = 16;
	params.Fullscreen = false;
	params.Stencilbuffer = stencil;
	params.Vsync = false;
	params.EventReceiver = nullptr;
	// params.WindowId = (void*)window->GetHandle();

	device = createDeviceEx(params);

	if (!device) {
		console->PostError("Could not create Irrlicht device", true);
		return false;
	}

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();

	HWND cur = GetHandle();
	if (cur) ShowWindow(cur, SW_HIDE);

	return true;
}

HWND Renderer::GetHandle() {
	if (!driver || !device) return nullptr;

	irr::video::SExposedVideoData data = driver->getExposedVideoData();
	if (data.D3D9.HWnd) return (HWND)data.D3D9.HWnd;
	if (data.D3D8.HWnd) return (HWND)data.D3D8.HWnd;
	if (data.OpenGLWin32.HWnd) return (HWND)data.OpenGLWin32.HWnd;
	return nullptr;
}

bool Renderer::Render() {
	// Camera queue, etc...
	// Simple logic for now:
	if (!device->run()) return false;

	driver->beginScene(true, true, bgColor);
	smgr->drawAll();
	driver->endScene();

	return true;
}