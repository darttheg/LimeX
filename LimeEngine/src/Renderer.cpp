#include "Renderer.h"

#include "DebugConsole.h"
#include "Window.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <fstream>
#include <stdexcept>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreConfigFile.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreStringConverter.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreCommon.h>
#include <OGRE/OgreFrameListener.h>

struct Renderer::FrameHook : public Ogre::FrameListener {
	Renderer* r = nullptr;
	explicit FrameHook(Renderer* rend) : r(rend) {}

	bool frameRenderingQueued(const Ogre::FrameEvent& evt) override {
		float dt = (float)evt.timeSinceLastFrame;
		if (dt > 0.1f) dt = 0.1f;
		r->deltaTime = dt;
		return true;
	}
};

Renderer::Renderer() {}
Renderer::~Renderer() {
	Close();
}

void Renderer::Close() {
	isCreated = false;
	o_SceneManager = nullptr;
	o_Window = nullptr;

	if (o_Root && hook)
		o_Root->removeFrameListener(hook.get());
	hook.reset();

	o_Root.reset();
}

void Renderer::SetDebugConsole(DebugConsole* d) {
	console = d;
}

void Renderer::SetWindow(Window* w) {
	window = w;
}

bool Renderer::Create() {
	if (isCreated) return true;

	try {
		o_Root = std::make_unique<Ogre::Root>("plugins.cfg");

		auto rs = o_Root->getAvailableRenderers();
		if (rs.empty()) throw std::runtime_error("No Ogre RenderSystems available.");
		o_Root->setRenderSystem(rs.front());

		o_Root->initialise(false);

		Ogre::NameValuePairList params;
		params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)window->GetHandle());
		params["vsync"] = "true";
		o_Window = o_Root->createRenderWindow("LimeOgre", 640, 480, false, &params);
		o_SceneManager = o_Root->createSceneManager();

		// Frame listener
		hook = std::make_unique<FrameHook>(this);
		o_Root->addFrameListener(hook.get());

		isCreated = true;
		return isCreated;
	} catch (const Ogre::Exception& e) {
		console->PostError(std::string("Ogre exception: ") + e.what(), true);
	} catch (const std::exception& e) {
		console->PostError(std::string("Failed to create renderer: ") + e.what(), true);
	}

	Close();
	return false;
}

bool Renderer::Render() {
	if (!isCreated || !o_Root || !o_Window) return false;
	if (o_Window->isClosed()) return false;

	o_Root->renderOneFrame();
	return true;
}

// ---

int Renderer::GetFrameRate() {
	return o_Window ? static_cast<int>(o_Window->getStatistics().lastFPS) : 0;
}

float Renderer::GetDeltaTime() {
	return deltaTime;
}

void Renderer::SetFrameLimit(int v) {
	
}

void Renderer::SetVSync(bool v) {
	o_Window->setVSyncEnabled(v);
}

int Renderer::GetElapsedTime() {
	return 0;
}