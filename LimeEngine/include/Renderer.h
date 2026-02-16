#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>
#include <memory>
#include <string>

class Vec2;
class Vec3;
class Vec4;
class Image;
class Receiver;

namespace irr {
	class IrrlichtDevice;
	namespace scene {
		class ISceneManager;
		class ISceneNode;
		class ICameraSceneNode;
	}
	namespace video {
		class IVideoDriver;
		class IGPUProgrammingServices;

		class ITexture;
		class IImage;
	}
	namespace gui {
		class IGUIEnvironment;
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
	bool RenderFromApp(); // App calls this function, checks if manualRendering is on to ignore render cycle or not.
	void RenderBGPreUpdate(); // For manual rendering, render the BG just so something is showing before the update loop.

	bool guardRenderingCheck();
	bool maximizeDevice();
	bool restoreDevice();
	bool isFocused();
	void applyLetterboxViewport(int fbW, int fbH, int baseW, int baseH);
	void updateRenderResolution(int w, int h);
	HWND getDeviceVideoData();
	bool isManualRenderingOn() { return manualRendering; }
	void setManualRendering(bool on) { manualRendering = on; }
	int getObjectCount();
	void setViewort(int x, int y, int w, int h);
	Vec4 getViewport();
	int updateFrameRate();

	// Scene
	Vec2 getRenderSize();
	void setRenderSize(const Vec2& size);
	int getElapsedTime();
	bool renderManually(bool clearBackBuffer = false, bool clearZBuffer = false); // User calls this function with manualRendering
	void setBackgroundColor(const Vec4& color);
	void setAmbientColor(const Vec4& color);
	void setShadowColor(const Vec4& color);
	void setFogColor(const Vec4& color);
	void setFogPlanes(const Vec2& planes);
	void setLightManagementType(int type);
	void setTextureCreationQuality(int quality);
	Image getErrorImage();
	void setMatchRes(bool v) { doMatchResolution = v; }
	bool getMatchRes() { return doMatchResolution; }

	// Textures
	irr::video::ITexture* createTexture(int w, int h, const std::string& name = "");
	irr::video::ITexture* createTexture(const std::string& path);
	irr::video::ITexture* cropTexture(irr::video::ITexture* tex, const Vec2& pos, const Vec2& dim);
	irr::video::ITexture* appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos);
	Vec4 getColor(irr::video::ITexture* tex, const Vec2& pos);
	irr::video::ITexture* setColor(irr::video::ITexture* tex, const Vec2& pos, const Vec4& color);
	void keyColor(irr::video::ITexture* tex, const Vec4& color);

	// Empty
	irr::scene::ISceneNode* createEmptyNode();

	// Skydome
	irr::scene::ISceneNode* createSkydomeNode(irr::video::ITexture* tex);

	// Camera
	irr::scene::ICameraSceneNode* createCameraNode();
	void updateCameraMatrix(irr::scene::ICameraSceneNode* c);
	void setActiveCamera(irr::scene::ICameraSceneNode* c);

	// Misc
	bool setMouseVisible(bool vis);
	bool setMousePosition(const Vec2& pos);

private:
	struct Vec4S { float x, y, z, w; };
	struct Vec2S { float x, y; };

	bool isCreated = false;
	bool hasBegunNewScene = false;

	// Renderer parameters
	Vec2S renderSize{};
	Vec4S bgColor{15,15,25,255};
	Vec4S fogColor{};
	Vec2S fogPlanes{};
	void updateFog();
	bool doMatchResolution = true; // Hook resolution w/h to window size
	bool manualRendering = false; // User-controlled rendering

	struct Viewport { int x = 0, y = 0, w = 0, h = 0; };
	Viewport vp;
	int mFbW = 0, mFbH = 0;

	// Irrlicht
	irr::IrrlichtDevice* i_device = nullptr;
	irr::scene::ISceneManager* i_smgr = nullptr;
	irr::video::IVideoDriver* i_driver = nullptr;
	irr::gui::IGUIEnvironment* i_gui = nullptr;
	irr::video::IGPUProgrammingServices* i_gpu = nullptr;
	int frameCount = 0;
	int lastTime = 0;
	int fps = 0;
};