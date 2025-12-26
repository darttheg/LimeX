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

Renderer::Renderer() {}
Renderer::~Renderer() {
	Close();
}

void Renderer::Close() {
	isCreated = false;
	o_Viewport = nullptr;
	o_Camera = nullptr;
	o_SceneManager = nullptr;
	o_Window = nullptr;
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
		o_Camera = o_SceneManager->createCamera("MainCamera");
		o_Camera->setNearClipDistance(0.1f);
		o_Camera->setAutoAspectRatio(true);

		o_Viewport = o_Window->addViewport(o_Camera);
		o_Viewport->setBackgroundColour(Ogre::ColourValue(0.1f, 0.1f, 0.12f));

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