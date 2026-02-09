#include "Modules/ModuleScene.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"

#include "Objects/Event.h"
#include "Objects/Image.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;

void Module::Scene::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Scene
	sol::table module = lua["Lime"]["Scene"].get_or_create<sol::table>();

	// Renders the scene using the active `Camera` and current rendering parameters. Clearing the back buffer will replace previously rendered items with the background color. Clearing the Z buffer ensures the previous depth pass is not used. (NOTE: Manual rendering must be on, otherwise this function has no effect. See `Lime.setManualRendering`.)
	// Params boolean? clearBackBuffer, boolean? clearZBuffer
	// Returns void
	module.set_function("render", &Module::Scene::Bind::RenderScene);

	// Returns the amount of 3D objects in the scene.
	// Returns number
	module.set_function("getObjectCount", &Module::Scene::Bind::GetObjectCount);

	// Sets the ambient color of the scene to `rgba`.
	// Params Vec4 rgba
	// Returns void
	module.set_function("setAmbientColor", &Module::Scene::Bind::SetAmbientColor);

	// Sets the background color of the scene to `rgba`. This color is generally only visible when there is no `Skydome`.
	// Params Vec4 rgba
	// Returns void
	module.set_function("setBackgroundColor", &Module::Scene::Bind::SetBackgroundColor);

	// Sets the color of the scene's fog to `rgba`.
	// Params Vec4 rgba
	// Returns void
	module.set_function("setFogColor", &Module::Scene::Bind::SetFogColor);

	// Sets where the scene's fog starts and ends.
	// Params Vec2 planes
	// Returns void
	module.set_function("setFogPlanes", &Module::Scene::Bind::SetFogPlanes);

	// Sets the color of shadows in the scene to `rgba`.
	// Params Vec4 rgba
	// Returns void
	module.set_function("setShadowColor", &Module::Scene::Bind::SetShadowColor);

	// Sets the light management type using `Lime.Enum.LightManagementType`.
	// Params Lime.Enum.LightManagementType type
	// Returns void
	module.set_function("setLightManagementType", &Module::Scene::Bind::SetLightManagementType);

	// Sets the default `Image` creation quality using `Lime.Enum.ImageCreationQuality`, where Low is optimized for speed and High is optimized for quality.
	// Params Lime.Enum.ImageCreationQuality
	// Returns void
	module.set_function("setImageCreationQuality", &Module::Scene::Bind::SetTextureCreationQuality);

	// Returns an `Image` of a lime and white checkerboard pattern, 2x2. Useful for missing Images and the like.
	// Returns Image
	module.set_function("getErrorImage", &Module::Scene::Bind::GetErrorImage);

	// If set to true, the rendering resolution will be that of the window resolution, updating on any window size changes. If false, the rendering resolution will remain the same size but upscale without anti-aliasing to the window resolution.
	// Params boolean doRescale
	// Returns void
	module.set_function("setRescaleRenderToWindowSize", &Module::Scene::Bind::SetMatchRenderResToWindow);

	// End Module
}

// Functions

bool Module::Scene::Bind::RenderScene(bool clearBackBuffer, bool clearZBuffer) {
	return r->renderManually(clearBackBuffer, clearZBuffer);
}

int Module::Scene::Bind::GetObjectCount() {
	return r->getObjectCount();
}

void Module::Scene::Bind::SetBackgroundColor(const Vec4& color) {
	r->setBackgroundColor(color);
}

void Module::Scene::Bind::SetFogColor(const Vec4& color) {
	r->setFogColor(color);
}

void Module::Scene::Bind::SetFogPlanes(const Vec2& planes) {
	r->setFogPlanes(planes);
}

void Module::Scene::Bind::SetAmbientColor(const Vec4& color) {
	r->setAmbientColor(color);
}

void Module::Scene::Bind::SetShadowColor(const Vec4& color) {
	r->setShadowColor(color);
}

void Module::Scene::Bind::SetLightManagementType(int type) {
	r->setLightManagementType(type);
}

void Module::Scene::Bind::SetTextureCreationQuality(int quality) {
	r->setTextureCreationQuality(quality);
}

void Module::Scene::Bind::SetMatchRenderResToWindow(bool v) {
	r->setMatchRes(v);
}

Image Module::Scene::Bind::GetErrorImage() {
	return r->getErrorImage();
}
