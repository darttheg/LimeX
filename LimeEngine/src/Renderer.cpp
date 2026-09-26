#pragma once

#include "Renderer.h"
#include <fstream>
#include <stdexcept>
#include "irrlicht.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Window.h"
#include "Receiver.h"
#include "GUIManager.h"
#include "QuadRenderer.h"
#include "RenderHelper.h"
#include "PhysicsManager.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"
#include "Objects/Mesh.h"

#include "External/CGUIColoredText.h"
#include "External/CTextAnchorSceneNode.h"
#include "LightManager.h"

#include "Objects/ShaderMaterial.h"
#include "Objects/IrrShaderMat.h"
#include <stack>

#include <thread>

static Application* a = nullptr;
static DebugConsole* d = nullptr;
static Window* w = nullptr;
static Receiver* r = nullptr;

#include "Objects/Event.h"
struct ButtonPair {
	std::shared_ptr<Event> onHovered = nullptr;
	std::shared_ptr<Event> onPressed = nullptr;
};

Renderer::Renderer(Application* owner) {
	a = owner;
	d = a->GetDebugConsole();
	w = a->GetWindow();
	guiManager = new GUIManager(this, d);
	rh = new RenderHelper(d);
	qr = new QuadRenderer();
	physics = new PhysicsManager(this, d);
}

void Renderer::setReceiver(Receiver* re) {
	r = re;
}

Renderer::~Renderer() {
	Shutdown();
}

static irr::video::ITexture* getCheckerError(irr::video::IVideoDriver* driver) {
	irr::video::ITexture* checker = driver->getTexture("error");
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

static irr::video::ITexture* getAlphaBlank(irr::video::IVideoDriver* driver) {
	irr::video::ITexture* blank = driver->getTexture("blank");
	if (!blank) {
		const irr::video::SColor L(0, 0, 0, 0);
		irr::video::IImage* img = driver->createImage(irr::video::ECF_A1R5G5B5, irr::core::dimension2du(1, 1));
		blank = driver->addTexture("blank", img);
		img->drop();
	}
	return blank;
}

#ifdef __ANDROID__
#include "LimeAndroid.h"
bool mountResources(irr::IrrlichtDevice* device) {
	if (!device) return false;
	unsigned long size = 0;
	const unsigned char* data = LimeAndroid::getResourcesZip(&size);
	if (!data || size == 0) return false;
	auto* fs = device->getFileSystem();
	irr::io::IReadFile* mem = fs->createMemoryReadFile((void*)data, (irr::s32)size, "resources.zip", false);
	bool out = fs->addFileArchive(mem);
	mem->drop();
	return out;
}
#else
bool mountResources(irr::IrrlichtDevice* device) {
	if (!device) return false;

	HMODULE mod = nullptr;
	GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)&mountResources,
		&mod
	);

	HRSRC rsrc = FindResource(mod, MAKEINTRESOURCE(101), RT_RCDATA);
	if (!rsrc) return false;
	HGLOBAL res = LoadResource(mod, rsrc);
	if (!res) return false;
	void* data = LockResource(res);
	DWORD size = SizeofResource(mod, rsrc);

	auto* fs = device->getFileSystem();
	irr::io::IReadFile* mem = fs->createMemoryReadFile(
		data,
		size,
		"resources.zip",
		false
	);

	bool out = fs->addFileArchive(mem);
	mem->drop();
	return out;
}
#endif

bool Renderer::Init() {
	if (isCreated) return false;

	WindowConfig cfg = a->GetConfig();

	SIrrlichtCreationParameters params;
	params.DriverType = (irr::video::E_DRIVER_TYPE)cfg.driverType;
	if (doMatchResolution)
		params.WindowSize = irr::core::dimension2d<u32>(cfg.windowSize[0], cfg.windowSize[1]);
	else
		params.WindowSize = irr::core::dimension2d<u32>(cfg.renderSize[0], cfg.renderSize[1]);

	params.Bits = 32;
	params.Vsync = cfg.vSync;
	params.Fullscreen = cfg.fullscreen;
	params.Stencilbuffer = doStencilBuffer;
	params.UsePerformanceTimer = false;
	// params.WindowId = (void*)glfwGetWin32Window(w->getGLFWWindow());

#ifdef __ANDROID__
	params.WindowId = LimeAndroid::getApp();
	params.WindowSize = irr::core::dimension2d<u32>(w->getRawWinSize().getX(), w->getRawWinSize().getY());
	params.Fullscreen = true;
#endif
	i_device = irr::createDeviceEx(params);

	if (!i_device || !i_device->getVideoDriver()) {
		d->Warn("Failed to create device!");
		return false;
	}

	i_device->getLogger()->setLogLevel(irr::ELOG_LEVEL::ELL_WARNING);

#ifdef __ANDROID__
	renderSize.x = cfg.renderSize[0];
	renderSize.y = cfg.renderSize[1];
#else
	renderSize.x = doMatchResolution ? cfg.windowSize[0] : cfg.renderSize[0];
	renderSize.y = doMatchResolution ? cfg.windowSize[1] : cfg.renderSize[1];
#endif

	i_smgr = i_device->getSceneManager();
	i_driver = i_device->getVideoDriver();
	i_gui = i_device->getGUIEnvironment();
	i_gpu = i_driver->getGPUProgrammingServices();

	if (params.Stencilbuffer && !i_driver->queryFeature(irr::video::EVDF_STENCIL_BUFFER)) {
		d->Warn("Shadow Volumes rely on the stencil buffer, which is not a supported feature in this device.");
		doStencilBuffer = false;
	}

	i_device->setEventReceiver(a->GetReceiver());

	guiManager->SetGUIEnv(i_gui, i_device);

	lightManager = new CLightManager(i_smgr);
	i_smgr->setLightManager(0);
	i_smgr->setLightManager(lightManager);

	using namespace irr;
	using namespace video;
	
#ifndef __ANDROID__
	E_DRIVER_TYPE dout = (E_DRIVER_TYPE)cfg.driverType;

	bool nullWin = false;
	switch (dout) {
		case E_DRIVER_TYPE::EDT_DIRECT3D8:
			hwndIrr = (HWND)i_device->getVideoDriver()->getExposedVideoData().D3D8.HWnd;
			break;
		case E_DRIVER_TYPE::EDT_DIRECT3D9:
			hwndIrr = (HWND)i_device->getVideoDriver()->getExposedVideoData().D3D9.HWnd;
			break;
		case E_DRIVER_TYPE::EDT_BURNINGSVIDEO:
			hwndIrr = (HWND)i_device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd;
			break;
		case E_DRIVER_TYPE::EDT_SOFTWARE:
			hwndIrr = (HWND)i_device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd;
			break;
		case E_DRIVER_TYPE::EDT_OPENGL:
			hwndIrr = (HWND)i_device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd;
			break;
		default: // None
			nullWin = true;
			break;
	}

	if (!nullWin && hwndIrr) {
		ShowWindow(hwndIrr, SW_HIDE);

		SetParent(hwndIrr, w->GetHandle());
		SetWindowLongPtr(hwndIrr, GWL_STYLE, WS_CHILD | WS_VISIBLE);
		SetWindowPos(hwndIrr, 0, 0, 0, renderSize.x, renderSize.y, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		ShowWindow(w->GetHandle(), SW_RESTORE);
		SetForegroundWindow(w->GetHandle());
		SetActiveWindow(w->GetHandle());
		SendMessage(hwndIrr, WM_ACTIVATE, WA_ACTIVE, 0);
		SendMessage(hwndIrr, WM_SETFOCUS, 0, 0);
	}

	glfwSetWindowUserPointer(w->getGLFWWindow(), this);
	glfwSetWindowFocusCallback(w->getGLFWWindow(), [](GLFWwindow* w, int focused) {
		auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(w));
		if (!renderer || !renderer->i_device) return;

		// Switch on driver type
		HWND h = renderer->getHandle();
		if (!h) return;

		if (focused)
		{
			SendMessage(h, WM_ACTIVATE, WA_ACTIVE, 0);
			SendMessage(h, WM_SETFOCUS, 0, 0);
		}
		else
		{
			SendMessage(h, WM_ACTIVATE, WA_INACTIVE, 0);
			SendMessage(h, WM_KILLFOCUS, 0, 0);
		}
		});
#endif

	alphaBlankTex = getAlphaBlank(i_driver);
	checkerTex = getCheckerError(i_driver);

	qr->init(i_driver, i_gui);
	qr->setWindowResolution(w->getSize().getX(), w->getSize().getY());
	qr->setInternalResolution(renderSize.x, renderSize.y);

	rh->Init(i_device);
	rh->SetLuaState(&a->GetLuaState());

	if (!mountResources(i_device))
		d->Warn("INIT WARNING: Could not mount resources.zip!");

	errMesh = i_smgr->getMesh("meshes/error.obj");

	isCreated = true;
	setTextureCreationQuality(1); // Medium
	setLightManagementType(0); // EightNearest

	// Shaders
	if (i_driver->getDriverType() == irr::video::EDT_OPENGL)
		depthShader = new IrrShaderMaterial(i_driver, "shaders/depth.vsh", "shaders/depth.psh", irr::video::EMT_SOLID);
	else
		depthShader = new IrrShaderMaterial(i_driver, "shaders/depth.hlsl", "shaders/depth.hlsl", irr::video::EMT_SOLID);

	return true;
}

bool Renderer::InitPhysics() {
	return physics->Init(i_device);
}

bool Renderer::Shutdown() {
	if (!isCreated) return true;

	isCreated = false;
	i_device->closeDevice();

	return true;
}

bool Renderer::UpdatePhysics(float dt) {
	if (!isCreated) return false;
	return physics->Update(dt);
}

void Renderer::renderDepthPass(bool rawDraw) {
	if (!doDepthPass || !depthShader || !depthShader->isValid()) return;

	i_driver->setRenderTarget(qr->getDepthTexture(), true, true);

	if (auto* cam = i_smgr->getActiveCamera()) {
		depthShader->setUniformFloat("near", cam->getNearValue());
		depthShader->setUniformFloat("far", cam->getFarValue());
	}

	irr::video::SOverrideMaterial& over = i_driver->getOverrideMaterial();
	over.Material.MaterialType = static_cast<irr::video::E_MATERIAL_TYPE>(depthShader->getMaterialType());
	over.EnableFlags = EMF_MATERIAL_TYPE;
	over.EnablePasses = irr::scene::ESNRP_SOLID;
	over.Enabled = true;

	i_smgr->drawAll();
	if (rawDraw)
		i_driver->setRenderTarget(0, false, false);
	else
		qr->bindScene(false);
	//i_driver->draw2DImage(qr->getDepthTexture(), irr::core::position2di(0, 0));

	over.EnableFlags = 0;
	over.Enabled = false;
}

#include "Objects/IrrShadowVolume.h"
bool Renderer::Render(float dt, bool clearBackBuffer, bool clearZBuffer) {
	if (!guardRenderingCheck()) return false;
	if (!isCreated || !w->isOpen()) return false;

	if (!doRender) return true;

	//if (!i_smgr->getActiveCamera()) // Is it appropriate to prematurely not render even a background?
	//	return false;

	bool hasParents = false;
	if (rh->getActiveCamera()) {
		irr::scene::ISceneNode* curParent = rh->getActiveCamera()->getParent();
		while (curParent) {
			hasParents = true;
			curParent->updateAbsolutePosition();
			curParent = curParent->getParent();
		}

		if (hasParents) {
			rh->getActiveCamera()->updateAbsolutePosition();
			rh->getActiveCameraForward()->updateAbsolutePosition();
			rh->getActiveCameraLeft()->updateAbsolutePosition();

			rh->getActiveCamera()->setTarget(rh->getActiveCameraForward()->getAbsolutePosition());
		}
	}

	// Step physics
	updateFog(); // Update fog params pre-render
	addToDtTime(dt);

	if (doMatchResolution && i_smgr->getActiveCamera())
		i_smgr->getActiveCamera()->setAspectRatio(w->getWinAR());

	Vec2 target = getTargetSize();
	rh->setTargetSize(target.getX(), target.getY());
	if (auto* cam = i_smgr->getActiveCamera())
		rh->updateCameraMatrix(cam, target.getX(), target.getY());

	bool rawDraw = doMatchResolution && !qr->ppxActive();
	ShadowVolumeSceneNode::DrawingThisFrame = false;

	auto drawShadows = [&]() {
		if (doStencilBuffer && ShadowVolumeSceneNode::DrawingThisFrame) {
			video::SColor s = i_smgr->getShadowColor();
			i_driver->drawStencilShadow(false, s, s, s, s);
		}
		};

	bool isVMVisible = viewModelCamera && viewModelCamera->isVisible();
	if (rawDraw) {
		i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
		if (viewModelCamera) viewModelCamera->setVisible(false);
		i_smgr->drawAll();

		drawShadows();

		physics->RenderDebug();

		renderDepthPass(true);

		if (viewModelCamera) {
			viewModelCamera->setVisible(isVMVisible);
			renderViewModel();
		}

		if (qr->getUserTexture())
			i_driver->draw2DImage(qr->getUserTexture(), irr::core::position2di());

		guiManager->Render();
	} else {
		i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));

		qr->beginInternal();

		if (viewModelCamera) viewModelCamera->setVisible(false);
		i_smgr->drawAll(); // Draw scene objects to rtScene

		drawShadows();

		physics->RenderDebug();
		renderDepthPass(false);

		if (viewModelCamera) {
			viewModelCamera->setVisible(isVMVisible);
			renderViewModel();
		}
		
		qr->beginGUIPass();
		guiManager->Render(); // Draw GUI objects to rtGUI
		qr->endInternal();

		qr->presentToWindow();
	}

	i_driver->endScene();

	hasBegunNewScene = true;
	didRenderOnce = true;

	return true;
}

bool Renderer::RunDevice() {
	if (!i_device) return false;
	i_device->getTimer()->tick();

	return true;
}

void Renderer::PrepareRenderingPostInit() {
	updateWindowSize(w->getRawWinSize().getX(), w->getRawWinSize().getY());

	i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
	i_driver->endScene();
}

// ---

int getNumChildren(irr::scene::ISceneNode* node) {
	if (!node) return 0;
	int total = 1;
	for (auto* child : node->getChildren())
		total += getNumChildren(child);
	return total;
}

// ---

Vec2 Renderer::getRenderSize() {
	if (!guardRenderingCheck()) return Vec2();
	return Vec2(renderSize.x, renderSize.y);
}

void Renderer::setRenderSize(const Vec2& size) {
	if (!guardRenderingCheck()) return;

	if (doMatchResolution) {
		d->Warn("Changing the render size while the render size is set to match the window size will not show any effect! See `Lime.Scene.setRescaleRenderToWindowSize`.");
		return;
	}

	renderSize.x = size.getX();
	renderSize.y = size.getY();

	qr->setInternalResolution(renderSize.x, renderSize.y);
	w->setSizeLimit(renderSize.x, renderSize.y);
}

int Renderer::getElapsedTime() {
	return isCreated ? a->GetWindow()->getTime() : 0;
}

bool Renderer::guardRenderingCheck() {
	if (!isCreated) {
		std::string out = "Interaction with renderable components is forbidden until the Lime window has been created.";
		d->PostError(out, true, true);
		return false;
	}
	return true;
}

void Renderer::warnGarbageCollection(const std::string& path) {
	if (preloadedPaths.count(path)) return;
	a->warnGarbageCollection(path);
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

int Renderer::updateFrameRate() {
	u32 currentTime = a->GetWindow()->getTime();
	++frameCount;

	if (currentTime - lastTime >= 1000) {
		fps = frameCount / ((currentTime - lastTime) / 1000.0f);
		lastTime = currentTime;
		frameCount = 0;
	}

	return fps;
}

void Renderer::updateWindowSize(int w, int h) {
	qr->setWindowResolution(w, h);
#ifndef __ANDROID__
	MoveWindow(getHandle(), 0, 0, w, h, TRUE);
#endif
	qr->prepareToRecreateRt();
}

int Renderer::getDriverFrameRate() {
	return i_device ? i_device->getVideoDriver()->getFPS() : 0;
}

Vec2 Renderer::getTargetSize() {
	if (doMatchResolution) return Vec2(w->getSize().getX(), w->getSize().getY());
	return Vec2(renderSize.x, renderSize.y);
}

std::string Renderer::getMeshName(irr::scene::IAnimatedMesh* msh) {
	return msh ? i_smgr->getMeshCache()->getMeshName(msh).getPath().c_str() : "";
}

#ifndef __ANDROID__
HWND Renderer::getDeviceVideoData() {
	return i_device ? reinterpret_cast<HWND>(i_device->getVideoDriver()->getExposedVideoData().OpenGLWin32.HWnd) : nullptr;
}
#endif

int Renderer::getObjectCount() {
	if (!i_driver) return 0;
	auto* root = i_smgr->getRootSceneNode();
	if (!root) return 0;

	return getNumChildren(root) - 1;
}

int Renderer::getTextureCount() {
	return i_driver ? i_driver->getTextureCount() : 0;
}

int Renderer::getMeshCount() {
	return i_smgr ? i_smgr->getMeshCache()->getMeshCount() : 0;
}

void Renderer::setSceneRenderQuality(int q) {
	//if (!guardRenderingCheck()) return;
	qr->setSceneRenderQuality(q);
}

void Renderer::setPostProcessingShader(const ShaderMaterial& sm) {
	if (!guardRenderingCheck()) return;
	qr->setPostProcessingShader(sm.isValid() ? sm.getMaterialType() : -1, sm.isValid() ? sm.getInternalMat() : nullptr);
}

void Renderer::clearPostProcessingShader() {
	if (!guardRenderingCheck()) return;
	qr->clearPostProcessingShader();
}

void Renderer::setPostProcessingShaderGUI(const ShaderMaterial& sm) {
	if (!guardRenderingCheck()) return;
	qr->setPostProcessingShaderGUI(sm.isValid() ? sm.getMaterialType() : -1, sm.isValid() ? sm.getInternalMat() : nullptr);
}

void Renderer::clearPostProcessingShaderGUI() {
	if (!guardRenderingCheck()) return;
	qr->clearPostProcessingShaderGUI();
}

irr::scene::ICameraSceneNode* Renderer::getActiveCameraNode() {
	if (!guardRenderingCheck()) return nullptr;
	return i_smgr->getActiveCamera();
}

irr::video::ITexture* Renderer::createRenderTargetTexture(const Vec2& size, irr::scene::ICameraSceneNode* c, const std::string& name) {
	if (!guardRenderingCheck()) return nullptr;
	std::string outName = "rtt_live_" + std::to_string(rttc);
	if (!name.empty()) outName = "live_" + name;
	irr::video::ITexture* out = i_driver->addRenderTargetTexture(irr::core::dimension2du(size.getX(), size.getY()), outName.c_str());

	irr::scene::ICameraSceneNode* prev = i_smgr->getActiveCamera();
	if (!prev || !c) {
		d->Warn("Failed to create render target texture: No valid Camera");
		return nullptr;
	}
	irr::f32 prevAR = prev ? prev->getAspectRatio() : 0.0f;

	updateFog();
	i_smgr->setActiveCamera(c ? c : prev);
	rh->updateCameraMatrix(i_smgr->getActiveCamera(), size.getX(), size.getY());
	i_driver->setRenderTarget(out, true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
	i_smgr->drawAll();
	i_driver->setRenderTarget(nullptr, true, true, 0);

	i_smgr->setActiveCamera(prev);
	if (prev) prev->setAspectRatio(prevAR);

	irr::video::IImage* bakedImage = i_driver->createImage(out, irr::core::vector2di(0, 0), irr::core::dimension2du(size.getX(), size.getY()));
	irr::video::ITexture* baked = nullptr;
	if (bakedImage) {
		std::string bname = "rtt_" + std::to_string(rttc);
		if (!name.empty()) bname = name;
		baked = i_driver->addTexture(bname.c_str(), bakedImage);
		bakedImage->drop();
	}

	if (baked) i_driver->removeTexture(out);
	else d->Warn("Could not bake render texture. Returning live texture. WARNING: If the driver resets (on resize), live render textures are destroyed.");

	rttc++;
	return baked ? baked : out;
}

void Renderer::setUserTexture(const Texture& tex) {
	if (!guardRenderingCheck()) return;
	if (!tex.getTexture()) return;
	qr->setUserTexture(tex.getTexture());
}

void Renderer::clearUserTexture() {
	if (!guardRenderingCheck()) return;
	qr->clearUserTexture();
}

void Renderer::setViewModelCamera(irr::scene::ICameraSceneNode* cam) {
	if (!guardRenderingCheck()) return;
	viewModelCamera = cam;
}

void Renderer::renderViewModel() {
	Vec2 target = getTargetSize();
	rh->updateCameraMatrix(viewModelCamera, target.getX(), target.getY());

	const irr::u32 now = i_device->getTimer()->getTime();
	viewModelCamera->OnAnimate(now);

	i_driver->clearZBuffer();

	auto* prev = i_smgr->getActiveCamera();
	i_smgr->setActiveCamera(viewModelCamera);
	viewModelCamera->render();

	std::function<void(irr::scene::ISceneNode*)> draw = [&](irr::scene::ISceneNode* n) {
		if (!n->isVisible()) return;
		n->render();
		for (auto* ch : n->getChildren()) draw(ch);
	};
	for (auto* c : viewModelCamera->getChildren()) draw(c);

	i_smgr->setActiveCamera(prev);
}

bool Renderer::preloadMesh(const std::string path) {
	if (!guardRenderingCheck()) return false;
	bool ok = i_smgr->getMesh(path.c_str()) != nullptr;
	if (ok)
		preloadedPaths.insert(i_device->getFileSystem()->getAbsolutePath(path.c_str()).c_str());
	return ok;
}

bool Renderer::preloadTexture(const std::string path) {
	if (!guardRenderingCheck()) return false;
	bool ok = i_driver->getTexture(path.c_str()) != nullptr;
	if (ok)
		preloadedPaths.insert(i_device->getFileSystem()->getAbsolutePath(path.c_str()).c_str());
	return ok;
}

bool Renderer::purgeMesh(const std::string path) {
	if (!guardRenderingCheck()) return false;
	irr::scene::IMeshCache* c = i_smgr->getMeshCache();
	if (c->getMeshByName(path.c_str())) {
		irr::scene::IAnimatedMesh* m = c->getMeshByName(path.c_str());
		m->drop();
		removeMesh(m);
		preloadedPaths.erase(i_device->getFileSystem()->getAbsolutePath(path.c_str()).c_str());
		return true;
	}
	return false;
}

bool Renderer::purgeTexture(const std::string path) {
	if (!guardRenderingCheck()) return false;
	if (i_driver->findTexture(path.c_str())) {
		irr::video::ITexture* t = i_driver->findTexture(path.c_str());
		t->drop();
		removeTexture(t);
		preloadedPaths.erase(i_device->getFileSystem()->getAbsolutePath(path.c_str()).c_str());
		return true;
	}
	return false;
}

void Renderer::addToDeletionQueue(irr::scene::ISceneNode* node) {
	if (!i_smgr || !node) return;
	i_smgr->addToDeletionQueue(node);
}

bool Renderer::removeTexture(irr::video::ITexture* tex) {
	if (!i_driver || !tex) return false;

	if (tex == checkerTex || tex == alphaBlankTex) {
		d->Warn("UNSAFE TEXTURE REMOVAL: This Texture is used by Lime's renderer! It cannot be removed.");
		return false;
	}

	qr->clearUsedTextures(tex);

	bool safe = tex->getReferenceCount() == 1; // Texture obj still owns it so ref == 1

	if (!safe) {
		std::string out = "UNSAFE TEXTURE REMOVAL: Texture is being called for purging but has ";
		out += std::to_string(tex->getReferenceCount());
		out += " reference(s)! (";
		out += tex->getName().getPath().c_str();
		out += ")";
		d->Warn(out);

		// Scene
		core::array<ISceneNode*> stack;
		if (ISceneNode* root = i_smgr->getRootSceneNode()) stack.push_back(root);

		while (!stack.empty()) {
			ISceneNode* node = stack.getLast();
			stack.erase(stack.size() - 1);
			for (auto* c : node->getChildren()) stack.push_back(c);

			if (!(node->getType() == ESNT_MESH || node->getType() == ESNT_SKY_DOME)) continue;

			for (u32 i = 0; i < node->getMaterialCount(); ++i) {
				irr::video::SMaterial& mat = node->getMaterial(i);
				for (u32 l = 0; l < irr::video::MATERIAL_MAX_TEXTURES; ++l) {
					if (mat.getTexture(l) == tex)
						mat.setTexture(l, checkerTex);
				}

				mat.setFlag(irr::video::E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, false);
				mat.setFlag(irr::video::E_MATERIAL_FLAG::EMF_LIGHTING, false);
				mat.setFlag(irr::video::E_MATERIAL_FLAG::EMF_FOG_ENABLE, false);
			}
		}

		// GUI
		if (tex->getReferenceCount() > 1) {
			core::array<irr::gui::IGUIElement*> stackG;
			if (irr::gui::IGUIElement* root = i_gui->getRootGUIElement()) stackG.push_back(root);

			while (!stackG.empty()) {
				irr::gui::IGUIElement* node = stackG.getLast();
				stackG.erase(stackG.size() - 1);
				for (auto* c : node->getChildren()) stackG.push_back(c);

				if (!(node->getType() == irr::gui::EGUIET_IMAGE)) continue;

				auto* img = static_cast<irr::gui::IGUIImage*>(node);
				if (img->getImage() == tex) {
					img->setImage(checkerTex);
					img->setScaleImage(true);
				}
			}
		}
	}

	i_driver->removeTexture(tex);

	return true;
}

bool Renderer::removeMesh(irr::scene::IAnimatedMesh* mesh) {
	if (!i_smgr || !mesh) return false;

	bool safe = mesh->getReferenceCount() <= 1;
	int physicsCount = physics->getMeshUseCount(mesh);
	if (physicsCount > 0) {
		std::string out = "MESH PURGE REFUSED: Mesh ";
		out += i_smgr->getMeshCache()->getMeshName(mesh).getPath().c_str();
		out += " is used in ";
		out += std::to_string(physicsCount);
		out += " physics object";
		if (physicsCount > 1) out += "s";
		out += ".";
		d->Warn(out);
		return false;
	}

	irr::scene::IAnimatedMesh* fallback = i_smgr->getMesh("meshes/error.obj");

	if (!safe) {
		std::string name = "no_name";
		auto* cache = i_smgr->getMeshCache();
		if (cache) {
			const irr::u32 n = cache->getMeshCount();
			for (irr::u32 i = 0; i < n; ++i) {
				if (cache->getMeshByIndex(i) == mesh) {
					name = cache->getMeshName(i).getPath().c_str();
				}
			}
		}

		std::string out = "UNSAFE MESH REMOVAL: Mesh is being called for purging but has ";
		out += std::to_string(mesh->getReferenceCount());
		out += " reference";
		if (mesh->getReferenceCount() > 1) out += "s";
		out += "! (";
		out += name;
		out += ")";
		d->Warn(out);
		core::array<ISceneNode*> stack;
		if (ISceneNode* root = i_smgr->getRootSceneNode()) stack.push_back(root);

		while (!stack.empty()) {
			ISceneNode* node = stack.getLast();
			stack.erase(stack.size() - 1);
			if (!node) continue;
			for (auto* c : node->getChildren()) stack.push_back(c);
			if (node->getType() != irr::scene::ESNT_ANIMATED_MESH) continue;

			auto* am = static_cast<irr::scene::IAnimatedMeshSceneNode*>(node);
			if (am && am->getMesh() == mesh) {
				am->setMesh(fallback);
				am->setMaterialTexture(0, checkerTex);

				am->setMaterialFlag(irr::video::E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, false);
				am->setMaterialFlag(irr::video::E_MATERIAL_FLAG::EMF_LIGHTING, false);
				am->setMaterialFlag(irr::video::E_MATERIAL_FLAG::EMF_FOG_ENABLE, false);
			}
		}
	}

	if (auto* cache = i_smgr->getMeshCache()) {
		cache->removeMesh(mesh);
		//cache->clearUnusedMeshes();
	}

	return true;
}

bool Renderer::removeBuffer(irr::scene::IMeshBuffer* buf) {
	if (!buf) return false;
	if (buf->getReferenceCount() > 1) {
		std::string out = "UNSAFE MESHBUFFER REMOVAL: MeshBuffer is being called for purging but has ";
		out += std::to_string(buf->getReferenceCount());
		out += " reference(s)! (";
		out += buf->getDebugName();
		out += ")";
		d->Warn(out);
	}

	buf->drop();
	return true;
}

void Renderer::clearScene() {
	if (!guardRenderingCheck()) return;
	int count = 0;
	std::vector<irr::scene::ISceneNode*> nodes;
	std::stack<irr::scene::ISceneNode*> stack;

	stack.push(i_smgr->getRootSceneNode());
	while (!stack.empty()) {
		irr::scene::ISceneNode* n = stack.top();
		stack.pop();
		for (auto* child : n->getChildren())
			stack.push(child);
		nodes.push_back(n);
	}

	count = nodes.size();

	for (auto it : nodes) {
		int c = it->getReferenceCount();
		for (int i = 0; i < c; i++)
			it->drop();
		it->remove();
		it = nullptr;
	}

	d->Warn("Cleared the scene of " + std::to_string(count) + " objects.");
}

void Renderer::setGUIQuality(int q) {
	auto setFilters = [&](bool bilinear, bool trilinear, u32 aniso) {
		for (int i = 0; i < 2; i++) {
			i_driver->getMaterial2D().TextureLayer[0].AnisotropicFilter = aniso;
			i_driver->getMaterial2D().TextureLayer[0].BilinearFilter = bilinear;
			i_driver->getMaterial2D().TextureLayer[0].TrilinearFilter = trilinear;
		}
	};

	switch (q) {
	case 1: // Medium
		i_driver->getMaterial2D().AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_SIMPLE;
		setFilters(true, false, 0);
		break;
	case 2: // High
		i_driver->getMaterial2D().AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_QUALITY;
		setFilters(true, true, 16);
		break;
	default: // Low
		i_driver->getMaterial2D().AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_OFF;
		setFilters(false, false, 0);
		break;
	}
}

Vec2 Renderer::getMousePosCorrected(float x, float y) {
	if (doMatchResolution) return Vec2(x, y);
	irr::core::recti vp = qr->getViewport();

	const int vpW = vp.getWidth() > 0 ? vp.getWidth() : (int)w->getSize().getX();
	const int vpH = vp.getHeight() > 0 ? vp.getHeight() : (int)w->getSize().getY();

	int vMouseX = (x - vp.UpperLeftCorner.X) * renderSize.x / (float)vpW;
	int vMouseY = (y - vp.UpperLeftCorner.Y) * renderSize.y / (float)vpH;

	/*
	int vMouseX = (x - vp.UpperLeftCorner.X) * renderSize.x / w->getSize().getX();
	int vMouseY = (y - vp.UpperLeftCorner.Y) * renderSize.y / w->getSize().getY();
	*/

	return Vec2(vMouseX, vMouseY);
}

void Renderer::setAmbientColor(const Vec4& color) {
	if (!guardRenderingCheck()) return;

	i_smgr->setAmbientLight(irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}

void Renderer::setLetterboxing(bool v) {
	qr->setLetterboxing(v);
}

void Renderer::setBackgroundColor(const Vec4& color) {
	if (!guardRenderingCheck()) return;

	bgColor.x = color.getX();
	bgColor.y = color.getY();
	bgColor.z = color.getZ();
	bgColor.w = color.getW();

	qr->setClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
}

void Renderer::setLightManagementType(int type) {
	if (!guardRenderingCheck()) return;
	type = std::clamp<int>(type, 0, 2);
	CLightManager* out = static_cast<CLightManager*>(lightManager);
	if (out) out->setMode(type);
}

void Renderer::setTextureCreationQuality(int q) {
	if (!guardRenderingCheck()) return;

	q = std::clamp<int>(q, 0, 2);
	i_driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, q == 2);
	i_driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_SPEED, q == 0);
}

void Renderer::setDoMipmapGen(bool enable) {
	if (!guardRenderingCheck()) return;
	i_driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, enable);
}

bool Renderer::getDoMipmapGen() {
	if (!guardRenderingCheck()) return false;
	return i_driver->getTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS);
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

void Renderer::setFogPlanes(float n, float f) {
	if (!guardRenderingCheck()) return;

	fogPlanes.x = n;
	fogPlanes.y = f;
}

Texture Renderer::getErrorTexture() {
	if (!guardRenderingCheck()) return Texture();

	return Texture(checkerTex);
}

void Renderer::setMatchRes(bool v) {
	qr->setMatchWindowRender(v);
	doMatchResolution = v;
}

void Renderer::setIntegerScaling(bool v) {
	qr->setIntegerScaling(v);
}

bool Renderer::getMouseVisible() {
	if (!guardRenderingCheck()) return false;
	return i_device->getCursorControl()->isVisible();
}

bool Renderer::setMouseVisible(bool vis) {
	if (!guardRenderingCheck()) return false;
	i_device->getCursorControl()->setVisible(vis);
	return true;
}

bool Renderer::setMousePosition(const Vec2& pos) {
	if (!guardRenderingCheck()) return false;
	i_device->getCursorControl()->setPosition(irr::core::vector2di(pos.getX(), pos.getY()));
	a->GetReceiver()->setMousePosition(pos.getX() - 1, pos.getY() - 1);
	return true;
}

#include "Interfaces/Object2D.h"
void Renderer::addButtonPair(irr::gui::IGUIButton* button, ButtonPair pair) {
	guiManager->addButtonPair(button, pair);
}

void Renderer::removeButtonPair(irr::gui::IGUIButton* button) {
	guiManager->removeButtonPair(button);
}

bool Renderer::isElementHovered(irr::gui::IGUIElement* element) {
	if (!element) return false;
	Vec2 corrected = getMousePosCorrected(r->getMouseState().pos.x, r->getMouseState().pos.y);
	return element->isPointInside(irr::core::vector2di((irr::s32)corrected.getX(), (irr::s32)corrected.getY()));
}

irr::io::IFileSystem* const Renderer::getFileSystem() {
	return i_device ? i_device->getFileSystem() : nullptr;
}

bool Renderer::addArchive(const std::string path) {
	if (!guardRenderingCheck()) return false;
	return i_device->getFileSystem()->addFileArchive(path.c_str(), true, false, irr::io::EFAT_UNKNOWN);
}

void Renderer::setOnResize(int w, int h) {
	if (!i_device) return;
	i_device->getVideoDriver()->OnResize(irr::core::dimension2du(w, h));
}

void Renderer::setShaderParameter(const std::string& name, float v) {
	ShaderParam& p = shaderParams[name];
	p.data[0] = v;
	p.count = 1;
}

void Renderer::setShaderParameter(const std::string& name, const Vec2& v) {
	ShaderParam& p = shaderParams[name];
	p.data[0] = v.getX(); p.data[1] = v.getY();
	p.count = 2;
}

void Renderer::setShaderParameter(const std::string& name, const Vec3& v) {
	ShaderParam& p = shaderParams[name];
	p.data[0] = v.getX(); p.data[1] = v.getY(); p.data[2] = v.getZ();
	p.count = 3;
}

void Renderer::setShaderParameter(const std::string& name, const Vec4& v) {
	ShaderParam& p = shaderParams[name];
	p.data[0] = v.getX(); p.data[1] = v.getY(); p.data[2] = v.getZ(); p.data[3] = v.getW();
	p.count = 4;
}

void Renderer::clearShaderParameter(const std::string& name) {
	shaderParams.erase(name);
}

#include "Objects/Event.h"
bool Renderer::runEventFromGUI(std::shared_ptr<Event> e, std::function<void(const std::string&)> onError) {
	if (!e)
		return false;
	e.get()->engineRun([&](const std::string& msg) { d->PostError(msg, false, false); });
	return true;
}

struct TextureWriteJob {
	u32 width = 0;
	u32 height = 0;
	std::vector<u8> rgba;
	std::string path;
};

bool CopyTextureRGBA(irr::video::IVideoDriver* driver, irr::video::ITexture* tex, TextureWriteJob& out, const std::string& path) {
	if (!driver || !tex) return false;

	irr::video::IImage* image = driver->createImage(tex, irr::core::vector2di(), tex->getSize());
	if (!image) return false;

	const auto size = image->getDimension();

	out.width = size.Width;
	out.height = size.Height;
	out.path = path;
	out.rgba.resize(out.width * out.height * 4);

	for (u32 y = 0; y < out.height; ++y) {
		for (u32 x = 0; x < out.width; ++x) {
			const irr::video::SColor c = image->getPixel(x, y);
			const size_t i = (y * out.width + x) * 4;

			out.rgba[i + 0] = c.getRed();
			out.rgba[i + 1] = c.getGreen();
			out.rgba[i + 2] = c.getBlue();
			out.rgba[i + 3] = c.getAlpha();
		}
	}

	image->drop();
	return true;
}

#pragma warning(push)
#pragma warning(disable: 4996)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma warning(pop)

void WriteTextureJob(TextureWriteJob job) {
	stbi_write_png(job.path.c_str(), job.width, job.height, 4, job.rgba.data(), job.width * 4);
}

bool SaveTextureAsync(irr::video::IVideoDriver* driver, irr::video::ITexture* tex, const std::string& path) {
	TextureWriteJob job;

	if (!CopyTextureRGBA(driver, tex, job, path)) return false;

	std::thread([job = std::move(job)]() mutable {
		WriteTextureJob(std::move(job));
	}).detach();

	return true;
}

bool Renderer::writeTextureToPath(irr::video::ITexture* t, const std::string& path) {
	if (!guardRenderingCheck()) return false;
	if (!t) return false;

	return SaveTextureAsync(i_driver, t, path);
}

bool Renderer::writeMeshToPath(irr::scene::IMesh* m, const std::string& path) {
	if (!guardRenderingCheck()) return false;
	if (!m) return false;

	auto* writer = i_smgr->createMeshWriter(irr::scene::EMESH_WRITER_TYPE::EMWT_OBJ);
	if (!writer) return false;

	auto* file = i_device->getFileSystem()->createAndWriteFile(path.c_str());
	if (!file) {
		writer->drop();
		return false;
	}

	bool ok = writer->writeMesh(file, m);

	file->drop();
	writer->drop();

	return ok;
}

static void drawTiledRegion(irr::video::IVideoDriver* driver, irr::video::ITexture* tx,
	const irr::core::rect<irr::s32>& src,
	irr::s32 dstX, irr::s32 dstY, irr::s32 dstW, irr::s32 dstH)
{
	const irr::s32 tileW = src.getWidth();
	const irr::s32 tileH = src.getHeight();
	if (tileW <= 0 || tileH <= 0 || dstW <= 0 || dstH <= 0) return;

	for (irr::s32 y = 0; y < dstH; y += tileH) {
		const irr::s32 h = std::min(tileH, dstH - y);
		for (irr::s32 x = 0; x < dstW; x += tileW) {
			const irr::s32 w = std::min(tileW, dstW - x);
			irr::core::rect<irr::s32> s(src.UpperLeftCorner.X, src.UpperLeftCorner.Y,
				src.UpperLeftCorner.X + w, src.UpperLeftCorner.Y + h);
			irr::core::rect<irr::s32> d(dstX + x, dstY + y, dstX + x + w, dstY + y + h);
			driver->draw2DImage(tx, d, s, nullptr, nullptr, true);
		}
	}
}

irr::video::ITexture* Renderer::toNineSliceTexture(irr::video::ITexture* tx, int cornerMargin, const Vec2& size, const std::string& name) {
	if (!guardRenderingCheck()) return nullptr;
	bool fail = false;
	if (!tx) fail = true;
	if (cornerMargin <= 0) fail = true;
	if (size.getX() <= cornerMargin || size.getY() <= cornerMargin) fail = true;
	if (!didRenderOnce) fail = true;

	if (fail) {
		std::string out = "Failed to create nine slice of Texture ";
		if (tx) {
			out += tx->getName().getPath().c_str();
			out += ".";
		}

		if (!didRenderOnce) out = "Failed to create nine slice because the scene is not yet ready. Nine-slice textures must be created after the first rendered frame. Use Lime.onPostStart or Lime.onUpdate.";

		d->Warn(out);
		return nullptr;
	}

	irr::core::dimension2du srcSize = tx->getSize();
	irr::s32 sw = (irr::s32)srcSize.Width;
	irr::s32 sh = (irr::s32)srcSize.Height;
	irr::s32 dw = (irr::s32)size.getX();
	irr::s32 dh = (irr::s32)size.getY();
	irr::s32 m = cornerMargin;
	irr::s32 sR = sw - m, sB = sh - m;
	irr::s32 dR = dw - m, dB = dh - m;

	std::string liveName = "rtt_nineslice_live_" + std::to_string(rttc);
	irr::video::ITexture* rtt = i_driver->addRenderTargetTexture(irr::core::dimension2du(dw, dh), liveName.c_str(), irr::video::ECF_A8R8G8B8);
	if (!rtt) return nullptr;

	const irr::core::rect<irr::s32> prevVp = i_driver->getViewPort();
	i_driver->setRenderTarget(rtt, true, true, irr::video::SColor(0, 0, 0, 0));
	i_driver->setViewPort(irr::core::rect<irr::s32>(0, 0, dw, dh));
	using irr::core::rect;
	
	i_driver->draw2DImage(tx, rect<irr::s32>(0, 0, m, m), rect<irr::s32>(0, 0, m, m), nullptr, nullptr, true);
	i_driver->draw2DImage(tx, rect<irr::s32>(dR, 0, dw, m), rect<irr::s32>(sR, 0, sw, m), nullptr, nullptr, true);
	i_driver->draw2DImage(tx, rect<irr::s32>(0, dB, m, dh), rect<irr::s32>(0, sB, m, sh), nullptr, nullptr, true);
	i_driver->draw2DImage(tx, rect<irr::s32>(dR, dB, dw, dh), rect<irr::s32>(sR, sB, sw, sh), nullptr, nullptr, true);

	drawTiledRegion(i_driver, tx, rect<irr::s32>(m, 0, sR, m), m, 0, dR - m, m);
	drawTiledRegion(i_driver, tx, rect<irr::s32>(m, sB, sR, sh), m, dB, dR - m, dh - dB);
	drawTiledRegion(i_driver, tx, rect<irr::s32>(0, m, m, sB), 0, m, m, dB - m);
	drawTiledRegion(i_driver, tx, rect<irr::s32>(sR, m, sw, sB), dR, m, dw - dR, dB - m);
	drawTiledRegion(i_driver, tx, rect<irr::s32>(m, m, sR, sB), m, m, dR - m, dB - m);

	i_driver->setViewPort(prevVp);
	i_driver->setRenderTarget(nullptr, true, true, 0);

	irr::video::IImage* baked = i_driver->createImage(rtt, irr::core::vector2di(0, 0), irr::core::dimension2du(dw, dh));
	irr::video::ITexture* bakedtx = nullptr;
	if (baked) {
		std::string bakedName;
		if (name.empty())
			bakedName = "rtt_nineslice_" + std::to_string(rttc);
		else {
			bakedName = name;
			if (irr::video::ITexture* existing = i_driver->getTexture(bakedName.c_str()))
				removeTexture(existing);
		}
		bakedtx = i_driver->addTexture(bakedName.c_str(), baked);
		if (bakedtx)
			preloadedPaths.insert(bakedName);
		baked->drop();
	}
	i_driver->removeTexture(rtt);
	rttc++;

	return bakedtx;
}
