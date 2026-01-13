#include "Renderer.h"

#include "DebugConsole.h"
#include "Window.h"

#include <fstream>
#include <stdexcept>

Renderer::Renderer() {}
Renderer::~Renderer() {
	Shutdown();
}

bool Renderer::Init(DebugConsole* d, Window* w) {
	if (isCreated) return false;

	console = d;
	window = w;

	SIrrlichtCreationParameters params;
	params.DriverType = irr::video::E_DRIVER_TYPE::EDT_DIRECT3D9;
	params.WindowSize = irr::core::dimension2d<u32>(640, 480);
	params.Bits = 16;
	params.Fullscreen = false;

	i_device = irr::createDeviceEx(params);
	if (!i_device) return false;

	i_smgr = i_device->getSceneManager();
	i_driver = i_device->getVideoDriver();
	i_gui = i_device->getGUIEnvironment();
	i_gpu = i_driver->getGPUProgrammingServices();

	HWND hwndIrr = (HWND)i_device->getVideoDriver()->getExposedVideoData().D3D9.HWnd;
	ShowWindow(hwndIrr, SW_HIDE);

	SetParent(hwndIrr, window->GetHandle());
	SetWindowLongPtr(hwndIrr, GWL_STYLE, WS_CHILD | WS_VISIBLE);
	SetWindowPos(hwndIrr, 0, 0, 0, 640, 480, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	ShowWindow(window->GetHandle(), SW_RESTORE);
	SetForegroundWindow(window->GetHandle());
	SetActiveWindow(window->GetHandle());
	SendMessage(hwndIrr, WM_ACTIVATE, WA_ACTIVE, 0);
	SendMessage(hwndIrr, WM_SETFOCUS, 0, 0);

	glfwSetWindowUserPointer(window->getGLFWWindow(), this);
	glfwSetWindowFocusCallback(window->getGLFWWindow(), [](GLFWwindow* w, int focused) {
		auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
		if (!renderer || !renderer->i_device) return;

		// Switch on driver type
		HWND hwndIrr = (HWND)renderer->i_driver->getExposedVideoData().D3D9.HWnd;

		if (focused)
		{
			SendMessage(hwndIrr, WM_ACTIVATE, WA_ACTIVE, 0);
			SendMessage(hwndIrr, WM_SETFOCUS, 0, 0);
		}
		else
		{
			SendMessage(hwndIrr, WM_ACTIVATE, WA_INACTIVE, 0);
			SendMessage(hwndIrr, WM_KILLFOCUS, 0, 0);
		}
		});

	isCreated = true;
	return true;
}

bool Renderer::Shutdown() {
	isCreated = false;
	
	i_device->closeDevice();

	return true;
}

bool Renderer::Render() {
	if (!isCreated || !window->getGLFWWindow()) return false;

	i_driver->beginScene(true, true, irr::video::SColor(255, 25, 25, 30));
	i_smgr->drawAll();
	i_gui->drawAll();
	i_driver->endScene();

	return true;
}

// ---