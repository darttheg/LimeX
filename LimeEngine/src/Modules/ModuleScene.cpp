#include "Modules/ModuleScene.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"

#include "Objects/Event.h"
#include "Objects/Texture.h"
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

	// Sets the default `Texture` creation quality using `Lime.Enum.TextureCreationQuality`, where Low is optimized for speed and High is optimized for quality.
	// Params Lime.Enum.TextureCreationQuality
	// Returns void
	module.set_function("setTextureCreationQuality", &Module::Scene::Bind::SetTextureCreationQuality);

	// Returns an `Texture` of a lime and white checkerboard pattern, 2x2. Useful for missing Textures and the like.
	// Returns Texture
	module.set_function("getErrorTexture", &Module::Scene::Bind::GetErrorTexture);

	// If set to true, the rendering resolution will be that of the window resolution, updating on any window size changes. If false, the rendering resolution will remain the same size but upscale without anti-aliasing to the window resolution.
	// Params boolean doRescale
	// Returns void
	module.set_function("setRescaleRenderToWindowSize", &Module::Scene::Bind::SetMatchRenderResToWindow);

	// Sets the render size to `size`. If the render size is set to rescale to the window size then this change will not take effect. See `Lime.Scene.setRescaleRenderToWindowSize`.
	// Params Vec2 size
	// Returns void
	module.set_function("setSize", &Module::Scene::Bind::SetRenderSize);

	// End Module
}

// Functions

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

Texture Module::Scene::Bind::GetErrorTexture() {
	return r->getErrorTexture();
}

void Module::Scene::Bind::SetRenderSize(const Vec2& size) {
	r->setRenderSize(size);
}
