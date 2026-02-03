#include "Renderer.h"
#include <fstream>
#include <stdexcept>
#include "irrlicht.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Window.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

Renderer::Renderer(Application* owner) {
	a = owner;
	d = a->GetDebugConsole();
	w = a->GetWindow();
}
Renderer::~Renderer() {
	Shutdown();
}

bool Renderer::Init() {
	if (isCreated) return false;

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

	SetParent(hwndIrr, w->GetHandle());
	SetWindowLongPtr(hwndIrr, GWL_STYLE, WS_CHILD | WS_VISIBLE);
	SetWindowPos(hwndIrr, 0, 0, 0, 640, 480, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	ShowWindow(w->GetHandle(), SW_RESTORE);
	SetForegroundWindow(w->GetHandle());
	SetActiveWindow(w->GetHandle());
	SendMessage(hwndIrr, WM_ACTIVATE, WA_ACTIVE, 0);
	SendMessage(hwndIrr, WM_SETFOCUS, 0, 0);

	glfwSetWindowUserPointer(w->getGLFWWindow(), this);
	glfwSetWindowFocusCallback(w->getGLFWWindow(), [](GLFWwindow* w, int focused) {
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
	if (!isCreated || !w->getGLFWWindow()) return false;

	i_driver->beginScene(true, true, irr::video::SColor(255, 25, 25, 30));
	i_smgr->drawAll();
	i_gui->drawAll();
	i_driver->endScene();

	return true;
}

// ---

irr::video::IImage* texToImg(irr::video::IVideoDriver* driver, irr::video::ITexture* tex) {
	if (!tex) return nullptr;

	irr::core::dimension2d<u32> texSize = tex->getSize();
	irr::video::ECOLOR_FORMAT format = tex->getColorFormat();
	void* pixelData = tex->lock();
	if (!pixelData) return nullptr;

	irr::video::IImage* image = driver->createImageFromData(format, texSize, pixelData);
	tex->unlock();

	return image;
}

// ---

bool Renderer::guardRenderingCheck() {
	if (!isCreated) {
		d->Warn("The renderer is not yet created, but an attempt to create a renderable object was made.");
		return false;
	}
	return true;
}

irr::video::ITexture* Renderer::createTexture(int w, int h, const std::string& name) {
	if (!guardRenderingCheck()) return nullptr;

	std::string outName = name;
	if (name.empty()) {
		outName = "tex_" + std::to_string((int)(rand() / (double)RAND_MAX * 9999));
	}

	irr::video::ITexture* tex = i_driver->addTexture(irr::core::dimension2du(w, h), outName.c_str());
	if (!tex) {
		d->Warn("Could not create texture " + outName + " of size " + std::to_string(w) + "x" + std::to_string(h));
		return nullptr;
	}

	return tex;
}

irr::video::ITexture* Renderer::createTexture(const std::string& path) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::ITexture* tex = i_driver->getTexture(path.c_str());
	if (!tex) {
		d->Warn("Could not create texture from " + path);
		return nullptr;
	}
	
	return tex;
}

irr::video::ITexture* Renderer::cropTexture(irr::video::ITexture* tex, const Vec2& pos, const Vec2& dim) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* img = i_driver->createImage(tex, irr::core::vector2di(pos.getX(), pos.getY()), irr::core::dimension2du(dim.getX(), dim.getY()));
	std::string name = tex->getName().getInternalName().c_str();
	name += "_cropped";
	irr::video::ITexture* out = i_driver->addTexture(name.c_str(), img);

	return out;
}

irr::video::ITexture* Renderer::appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* srcBase = texToImg(i_driver, tex);
	irr::video::IImage* appendBase = texToImg(i_driver, toAppend);

	appendBase->copyTo(srcBase, irr::core::vector2di(pos.getX(), pos.getY()));
	std::string op = tex->getName().getInternalName().c_str();
	op += "_appended";

	irr::video::ITexture* out = i_driver->addTexture(op.c_str(), srcBase);
	srcBase->drop();
	appendBase->drop();

	return out;
}

Vec4 Renderer::getColor(irr::video::ITexture* tex, const Vec2& pos) {
	if (!guardRenderingCheck()) return Vec4();

	irr::video::IImage* img = texToImg(i_driver, tex);
	irr::video::SColor pCol = img->getPixel(pos.getX(), pos.getY());
	img->drop();

	return Vec4(pCol.getRed(), pCol.getGreen(), pCol.getBlue(), pCol.getAlpha());
}

irr::video::ITexture* Renderer::setColor(irr::video::ITexture* tex, const Vec2& pos, const Vec4& color) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* img = texToImg(i_driver, tex);
	img->getPixel(pos.getX(), pos.getY());

	irr::video::ITexture* out = i_driver->addTexture(tex->getName().getPath().c_str(), img);
	i_driver->removeTexture(tex);
	img->drop();

	return out;
}

void Renderer::keyColor(irr::video::ITexture* tex, const Vec4& color) {
	i_driver->makeColorKeyTexture(tex, irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}
