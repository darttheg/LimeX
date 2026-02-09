#include "Renderer.h"
#include <fstream>
#include <stdexcept>
#include "irrlicht.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Window.h"
#include "Receiver.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Image.h"

static Application* a = nullptr;
static DebugConsole* d = nullptr;
static Window* w = nullptr;

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
	// params.Stencilbuffer = true;
	params.EventReceiver = a->GetReceiver();

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

bool Renderer::Render(bool clearBackBuffer, bool clearZBuffer) {
	if (!guardRenderingCheck()) return false;
	if (!isCreated || !w->getGLFWWindow()) return false;

	if (manualRendering && !i_smgr->getActiveCamera()) {
		d->Warn("Cannot render without an active Camera!");
		return false;
	} else if (!i_smgr->getActiveCamera()) {
		return false;
	}

	updateFog(); // Update fog params pre-render
	if (manualRendering) {
		bool bb = clearBackBuffer;
		if (!hasBegunNewScene) bb = false;
		i_driver->beginScene(bb, clearZBuffer, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
		i_smgr->drawAll();
	} else {
		i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
		i_smgr->drawAll();
		i_gui->drawAll();
	}
	i_driver->endScene();

	hasBegunNewScene = true;

	return true;
}

bool Renderer::RenderFromApp() {
	if (!manualRendering) return Render();
	return true;
}

void Renderer::RenderBGPreUpdate() {
	if (!manualRendering) return;

	i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
	i_driver->endScene();
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

int getNumChildren(irr::scene::ISceneNode* node) {
	if (!node) return 0;
	int total = 1;
	for (auto* child : node->getChildren())
		total += getNumChildren(child);
	return total;
}

static irr::video::ITexture* getCheckerError(irr::video::IVideoDriver* driver) {
	irr::video::ITexture* checker = driver->getTexture("limeError");
	if (!checker) {
		const irr::video::SColor L(255, 153, 229, 80), W(255, 255, 255, 255);
		irr::video::IImage* img = driver->createImage(irr::video::ECF_R5G6B5, irr::core::dimension2du(2, 2));
		img->setPixel(0, 0, L); img->setPixel(1, 0, W);
		img->setPixel(0, 1, W); img->setPixel(1, 1, L);
		checker = driver->addTexture("error", img);
		img->drop();
	}
	return checker;
}

// ---

int Renderer::getElapsedTime() {
	return isCreated ? i_device->getTimer()->getTime() : 0;
}

bool Renderer::renderManually(bool clearBackBuffer, bool clearZBuffer) {
	if (!guardRenderingCheck()) return false;
	if (!manualRendering) {
		d->Warn("Manual rendering is not enabled. See Lime.setManualRendering.");
	}

	return Render(clearBackBuffer, clearZBuffer);
}

bool Renderer::guardRenderingCheck() {
	if (!isCreated) {
		d->Warn("Scene objects cannot be created until the Lime window has been created!");
		return false;
	}
	return true;
}

bool Renderer::maximizeDevice() {
	if (i_device)
		i_device->maximizeWindow();

	// updateRenderResolution(w->getSize().getX(), w->getSize().getY());

	return i_device;
}

bool Renderer::restoreDevice() {
	if (i_device)
		i_device->restoreWindow();

	// updateRenderResolution(w->getSize().getX(), w->getSize().getY());

	return i_device;
}

bool Renderer::isFocused() {
	return i_device ? i_device->isWindowFocused() : false;
}

void Renderer::updateRenderResolution(int w, int h) {
	SetWindowPos(getDeviceVideoData(), nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	if (doMatchResolution) {
		irr::core::dimension2d<u32> newSize(static_cast<u32>(w), static_cast<u32>(h));
		i_driver->OnResize(newSize);
	}

	if (i_gui) {
		auto* root = i_gui->getRootGUIElement();
		root->setRelativePosition(core::rect<s32>(0, 0, (s32)w, (s32)h));
	}

	if (i_smgr->getActiveCamera())
	{
		i_smgr->getActiveCamera()->setAspectRatio((f32)w / (f32)h);
	}
}

HWND Renderer::getDeviceVideoData() {
	return i_device ? reinterpret_cast<HWND>(i_device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd) : nullptr;
}

int Renderer::getObjectCount() {
	if (!i_driver) return 0;
	auto* root = i_smgr->getRootSceneNode();
	if (!root) return 0;

	return getNumChildren(root) - 1;
}

void Renderer::setViewort(int x, int y, int w, int h) {
	if (!i_device) return;

	i_driver->setViewPort(irr::core::rect<irr::s32>(
		x,
		y,
		x + w,
		y + h
	));
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
	std::string name = "cropped_";
	name += tex->getName().getInternalName().c_str();
	irr::video::ITexture* out = i_driver->addTexture(name.c_str(), img);

	return out;
}

irr::video::ITexture* Renderer::appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* srcBase = texToImg(i_driver, tex);
	irr::video::IImage* appendBase = texToImg(i_driver, toAppend);

	appendBase->copyTo(srcBase, irr::core::vector2di(pos.getX(), pos.getY()));
	std::string op = "appended_";
	op += tex->getName().getInternalName().c_str();

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
	if (!guardRenderingCheck()) return;

	i_driver->makeColorKeyTexture(tex, irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}

irr::scene::ISceneNode* Renderer::createEmptyNode() {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addEmptySceneNode();
}

irr::scene::ISceneNode* Renderer::createSkydomeNode(irr::video::ITexture* tex) {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addSkyDomeSceneNode(tex, 16, 8, 1.0, 2.0, 100.0);
}

irr::scene::ICameraSceneNode* Renderer::createCameraNode() {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addCameraSceneNode();
}

void Renderer::updateCameraMatrix(irr::scene::ICameraSceneNode* c) {
	if (!c) return;

	if (c->isTrulyOrthogonal) {
		irr::core::matrix4 orthoMat;
		float z = c->getFOV() * 180.0 / irr::core::PI / 5.0;
		int width = i_driver->getScreenSize().Width;
		int height = i_driver->getScreenSize().Height;
		orthoMat.buildProjectionMatrixOrthoLH(width / z, height / z, c->getNearValue(), c->getFarValue());
		c->setProjectionMatrix(orthoMat, true);
	} else {
		irr::core::matrix4 perspectiveMat;
		float aspectRatio = c->getAspectRatio();
		perspectiveMat.buildProjectionMatrixPerspectiveFovLH(c->getFOV(), aspectRatio, c->getNearValue(), c->getFarValue());

		c->setProjectionMatrix(perspectiveMat, false);
	}
}

void Renderer::setActiveCamera(irr::scene::ICameraSceneNode* c) {
	if (!guardRenderingCheck()) return;

	i_smgr->setActiveCamera(c);
}

void Renderer::setAmbientColor(const Vec4& color) {
	if (!guardRenderingCheck()) return;

	i_smgr->setAmbientLight(irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}

void Renderer::setBackgroundColor(const Vec4& color) {
	if (!guardRenderingCheck()) return;

	bgColor.x = color.getX();
	bgColor.y = color.getY();
	bgColor.z = color.getZ();
	bgColor.w = color.getW();
}

void Renderer::setLightManagementType(int type) {
	if (!guardRenderingCheck()) return;

	// TODO
}

void Renderer::setTextureCreationQuality(int quality) {
	if (!guardRenderingCheck()) return;

	// TODO
}

void Renderer::setShadowColor(const Vec4& color) {
	if (!guardRenderingCheck()) return;

	i_smgr->setShadowColor(irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}

void Renderer::updateFog() {
	i_driver->setFog(irr::video::SColor(fogColor.w, fogColor.x, fogColor.y, fogColor.z), irr::video::EFT_FOG_LINEAR, fogPlanes.x, fogPlanes.y);
}

void Renderer::setFogColor(const Vec4& color) {
	if (!guardRenderingCheck()) return;

	fogColor.x = color.getX();
	fogColor.y = color.getY();
	fogColor.z = color.getZ();
	fogColor.w = color.getW();
}

void Renderer::setFogPlanes(const Vec2& planes) {
	if (!guardRenderingCheck()) return;

	fogPlanes.x = planes.getX();
	fogPlanes.y = planes.getY();
}

Image Renderer::getErrorImage() {
	if (!guardRenderingCheck()) return Image();

	return Image(getCheckerError(i_driver));
}