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

namespace irr {
	class IrrlichtDevice;
	namespace scene {
		class ISceneManager;
		class ISceneNode;
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
	bool Render();

	bool guardRenderingCheck();

	// Textures
	irr::video::ITexture* createTexture(int w, int h, const std::string& name = "");
	irr::video::ITexture* createTexture(const std::string& path);
	irr::video::ITexture* cropTexture(irr::video::ITexture* tex, const Vec2& pos, const Vec2& dim);
	irr::video::ITexture* appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos);
	Vec4 getColor(irr::video::ITexture* tex, const Vec2& pos);
	irr::video::ITexture* setColor(irr::video::ITexture* tex, const Vec2& pos, const Vec4& color);
	void keyColor(irr::video::ITexture* tex, const Vec4& color);

	// Skydome
	irr::scene::ISceneNode* createSkydomeNode(irr::video::ITexture* tex);

private:
	Application* a = nullptr;
	DebugConsole* d = nullptr;
	Window* w = nullptr;

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