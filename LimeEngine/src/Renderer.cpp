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

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"

#include "External/CGUIColoredText.h"
#include "External/CTextAnchorSceneNode.h"

static Application* a = nullptr;
static DebugConsole* d = nullptr;
static Window* w = nullptr;
static Receiver* r = nullptr;

Renderer::Renderer(Application* owner) {
	a = owner;
	d = a->GetDebugConsole();
	w = a->GetWindow();
	r = a->GetReceiver();
	guiManager = new GUIManager(d, this);
	rh = new RenderHelper();
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
	irr::video::ITexture* blank = driver->getTexture("alpha");
	if (!blank) {
		const irr::video::SColor L(0, 0, 0, 0);
		irr::video::IImage* img = driver->createImage(irr::video::ECF_A1R5G5B5, irr::core::dimension2du(1, 1));
		blank = driver->addTexture("blank", img);
		img->drop();
	}
	return blank;
}

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

bool Renderer::Init() {
	if (isCreated) return false;

	WindowConfig cfg = a->GetConfig();

	SIrrlichtCreationParameters params;
	params.DriverType = (irr::video::E_DRIVER_TYPE)cfg.driverType;
	params.WindowSize = irr::core::dimension2d<u32>(cfg.renderSize[0], cfg.renderSize[1]);
	params.Bits = 16;
	params.Vsync = cfg.vSync;
	params.Fullscreen = cfg.fullscreen;
	params.Stencilbuffer = true;

	i_device = irr::createDeviceEx(params);
	if (!i_device) return false;

	i_device->getLogger()->setLogLevel(irr::ELOG_LEVEL::ELL_WARNING);

	renderSize.x = cfg.renderSize[0];
	renderSize.y = cfg.renderSize[1];

	i_smgr = i_device->getSceneManager();
	i_driver = i_device->getVideoDriver();
	i_gui = i_device->getGUIEnvironment();
	i_gpu = i_driver->getGPUProgrammingServices();

	i_device->setEventReceiver(a->GetReceiver());
	a->GetReceiver()->initJoysticks(i_device);

	guiManager->SetGUIEnv(i_gui);

	using namespace irr;
	using namespace video;
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

	qr = new QuadRenderer();
	qr->init(i_driver);
	qr->setInternalResolution(renderSize.x, renderSize.y);

	rh->Init(i_device, d);
	rh->SetLuaState(&a->GetLuaState());

	if (!mountResources(i_device))
		d->Warn("INIT WARNING: Could not mount resources.zip!");

	alphaBlankTex = getAlphaBlank(i_driver);
	checkerTex = getCheckerError(i_driver);
	errMesh = i_smgr->getMesh("meshes/error.obj");

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

	/* Doesn't really do anything crazy
	if (!w->isFocused()) {
		i_device->yield();
		return true;
	}
	*/

	if (!i_device->run()) {
		d->Warn("Render device could not be ran!");
		return false;
	}

	if (!doRender) return true;

	//if (!i_smgr->getActiveCamera()) // Is it appropriate to prematurely not render even a background?
	//	return false;

	updateFog(); // Update fog params pre-render

	if (doMatchResolution && i_smgr->getActiveCamera()) {
		i_smgr->getActiveCamera()->setAspectRatio(w->getWinAR());
	}

	if (doMatchResolution) {
		i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
		i_smgr->drawAll();
		guiManager->Render();
		i_driver->endScene();
	} else {
		qr->beginInternal();
		i_smgr->drawAll(); // Draw scene objects to rtScene
		qr->beginGUIPass();
		guiManager->Render(); // Draw GUI objects to rtGUI
		qr->endInternal();

		i_driver->beginScene(true, true, irr::video::SColor(bgColor.w, bgColor.x, bgColor.y, bgColor.z));
		qr->presentToWindow();
		i_driver->endScene();
	}

	hasBegunNewScene = true;

	return true;
}

void Renderer::RenderBGPreUpdate() {
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
		d->Warn("Rendered objects cannot be created until the Lime window has been created!");
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
	MoveWindow(getHandle(), 0, 0, w, h, TRUE);
	qr->prepareToRecreateRt();
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

void Renderer::setSceneRenderQuality(int q) {
	if (!guardRenderingCheck()) return;

	qr->setSceneRenderQuality(q);
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

	bool safe = mesh->getReferenceCount() == 0;

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
		out += " reference(s)! (";
		out += name;
		out += ")";
		d->Warn(out);
		core::array<ISceneNode*> stack;
		if (ISceneNode* root = i_smgr->getRootSceneNode()) stack.push_back(root);

		while (!stack.empty()) {
			ISceneNode* node = stack.getLast();
			stack.erase(stack.size() - 1);
			for (auto* c : node->getChildren()) stack.push_back(c);

			if (!node->getType() == irr::scene::ESNT_ANIMATED_MESH) continue;

			auto* am = static_cast<irr::scene::IAnimatedMeshSceneNode*>(node);
			if (am->getMesh() == mesh) {
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
		cache->clearUnusedMeshes();
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

	qr->setClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
}

void Renderer::setLightManagementType(int type) {
	if (!guardRenderingCheck()) return;

	/*
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	*/
}

void Renderer::setTextureCreationQuality(int quality) {
	if (!guardRenderingCheck()) return;

	/*
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	TODO
	*/
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

Texture Renderer::getErrorTexture() {
	if (!guardRenderingCheck()) return Texture();

	return Texture(checkerTex);
}

void Renderer::setMatchRes(bool v) {
	qr->setMatchWindowRender(v);
	doMatchResolution = v;
}

bool Renderer::setMouseVisible(bool vis) {
	if (!i_device) return false;
	i_device->getCursorControl()->setVisible(vis);
	return true;
}

bool Renderer::setMousePosition(const Vec2& pos) {
	if (!i_device) return false;
	i_device->getCursorControl()->setPosition(irr::core::vector2di(pos.getX(), pos.getY()));
	a->GetReceiver()->setMousePosition(pos.getX() - 1, pos.getY() - 1);
	a->GetReceiver()->syncMouse();
	return true;
}

#include "Interfaces/Object2D.h"
void Renderer::addButtonPair(const Object2D& o) {
	guiManager->addButtonPair(o);
}

void Renderer::removeButtonPair(const Object2D& o) {
	guiManager->removeButtonPair(o);
}

bool Renderer::isElementHovered(const Object2D& o) {
	if (!o.getNode()) return false;
	return o.getNode()->isPointInside(irr::core::vector2di(r->getMouseState().pos.x, r->getMouseState().pos.y));
}

#include "Objects/Event.h"
bool Renderer::runEventFromGUI(std::shared_ptr<Event> e, std::function<void(const std::string&)> onError) {
	if (!e)
		return false;
	e.get()->engineRun(a->GetLuaState(), [&](const std::string& msg) { d->PostError(msg); });
	return true;
}