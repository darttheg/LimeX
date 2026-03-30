#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>
#include <memory>
#include <string>
#include <functional>

class Vec2;
class Vec3;
class Vec4;
class Texture;
class Receiver;
class GUIManager;
class Event;
class Object2D;
class QuadRenderer;
class RenderHelper;
class ShaderMaterial;

namespace irr {
	class IrrlichtDevice;
	namespace scene {
		class ISceneManager;
		class ISceneNode;
		class ICameraSceneNode;
		class IBillboardSceneNode;
		class CTextAnchorSceneNode;
		class IAnimatedMesh;
		class IMeshBuffer;
		class ILightManager;
	}
	namespace video {
		class IVideoDriver;
		class IGPUProgrammingServices;
		class ITexture;
	}
	namespace gui {
		class IGUIEnvironment;
		class CGUIColoredText;
		class IGUIStaticText;
		class IGUIImage;
		class IGUIButton;
		class IGUIElement;
	}
	namespace io {
		class IFileSystem;
	}
}
using namespace irr;

class Application;
class DebugConsole;
class Window;

class Renderer {
public:
	Renderer(Application* owner);
	~Renderer();

	bool Init();
	bool Shutdown();
	bool Render(bool clearBackBuffer = false, bool clearZBuffer = true);
	void EndWholeScene() { hasBegunNewScene = false; }
	bool RunDevice();
	void RenderBGPreUpdate(); // For manual rendering, render the BG just so something is showing before the update loop.
	RenderHelper* GetRenderHelper() const { return rh; }

	bool guardRenderingCheck();
	void warnGarbageCollection(const std::string& path);
	bool maximizeDevice();
	bool restoreDevice();
	bool isFocused();
	HWND getDeviceVideoData();
	int getObjectCount();
	int updateFrameRate();
	void updateWindowSize(int w, int h);
	int getDriverFrameRate();

	// Scene
	Vec2 getRenderSize();
	void setRenderSize(const Vec2& size);
	int getElapsedTime();
	void setBackgroundColor(const Vec4& color);
	void setAmbientColor(const Vec4& color);
	void setShadowColor(const Vec4& color);
	void setFogColor(const Vec4& color);
	void setFogPlanes(const Vec2& planes);
	void setLightManagementType(int type);
	void setTextureCreationQuality(int quality);
	Texture getErrorTexture();
	void setMatchRes(bool v);
	bool getMatchRes() { return doMatchResolution; }
	void setSceneRenderQuality(int q);
	void setPostProcessingShader(const ShaderMaterial& sm);
	void clearPostProcessingShader();
	irr::scene::ICameraSceneNode* getActiveCameraNode();
	irr::video::ITexture* createRenderTargetTexture(const Vec2& size, irr::scene::ICameraSceneNode* c);

	// Preloading - Add to queue and load one mesh+texture per frame
	// bool preloadMesh(sol::variadic_args va);
	// bool preloadTexture(sol::variadic_args va);

	// Scene - Cleanup
	void addToDeletionQueue(irr::scene::ISceneNode* node);
	bool removeTexture(irr::video::ITexture* tex);
	bool removeMesh(irr::scene::IAnimatedMesh* mesh);
	bool removeBuffer(irr::scene::IMeshBuffer* buf);
	void clearScene(); // Dangerous, don't expose

	// GUI
	void setGUIQuality(int q);
	Vec2 getMousePosCorrected(float x, float y);

	// Misc
	bool setMouseVisible(bool vis);
	bool setMousePosition(const Vec2& pos);
	HWND getHandle() { return hwndIrr; }
	GUIManager* getGUIManager() { return guiManager; }
	bool runEventFromGUI(std::shared_ptr<Event> e, std::function<void(const std::string&)> onError);
	void addButtonPair(const Object2D& o);
	void removeButtonPair(const Object2D& o);
	bool isElementHovered(const Object2D& o);
	void setActivelyRendering(bool v) { doRender = v; }
	bool getIsActivelyRendering() const { return doRender; }
	irr::video::IVideoDriver* const getVideoDriver() { return i_driver; }
	irr::io::IFileSystem* const getFileSystem();
	bool addArchive(const std::string path);
	void setOnResize(int w, int h);

	// Time
	void addToDtTime(float dt) { dtTime += dt; }
	float getDtTime() const { return dtTime; }

private:
	struct Vec4S { float x, y, z, w; };
	struct Vec2S { float x, y; };

	bool isCreated = false;
	bool hasBegunNewScene = false;
	bool doRender = true;

	float dtTime = 0.0f;

	// Renderer parameters
	Vec2S renderSize{};
	// Vec4S bgColor{15,15,25,255};
	Vec4S bgColor{ 0,0,0,255 };
	Vec4S fogColor{};
	Vec2S fogPlanes{};
	void updateFog();
	bool doMatchResolution = true; // Hook resolution w/h to window size

	// Irrlicht
	irr::IrrlichtDevice* i_device = nullptr;
	irr::scene::ISceneManager* i_smgr = nullptr;
	irr::video::IVideoDriver* i_driver = nullptr;
	irr::gui::IGUIEnvironment* i_gui = nullptr;
	irr::video::IGPUProgrammingServices* i_gpu = nullptr;
	int frameCount = 0;
	int lastTime = 0;
	int fps = 0;
	int rttc = 0;
	HWND hwndIrr = nullptr;
	QuadRenderer* qr = nullptr;
	RenderHelper* rh = nullptr;
	irr::scene::ILightManager* lightManager = nullptr;

	// Fallback
	irr::video::ITexture* alphaBlankTex = nullptr;
	irr::video::ITexture* checkerTex = nullptr;
	irr::scene::IAnimatedMesh* errMesh = nullptr;

	// GUI Environment
	GUIManager* guiManager = nullptr;
};