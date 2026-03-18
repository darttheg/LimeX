#include "Modules/ModuleScene.h"

#include "DebugConsole.h"
#include "Application.h"
#include "Renderer.h"
#include "RenderHelper.h"

#include "Objects/Event.h"
#include "Objects/Texture.h"
#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Mesh.h"

#include <sol/sol.hpp>

static DebugConsole* d;
static Application* a;
static Renderer* r;
static RenderHelper* rh;

void Module::Scene::bind(Application* app) {
	a = app;
	d = app->GetDebugConsole();
	r = app->GetRenderer();
	rh = app->GetRenderHelper();
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

	// Sets the light management behavior using `Lime.Enum.LightManagementType`. 
	// Params Lime.Enum.LightManagementType type
	// Returns void
	module.set_function("setLightManagementType", &Module::Scene::Bind::SetLightManagementType);

	// Sets the default `Texture` creation quality using `Lime.Enum.TextureCreationQuality`, where Low is optimized for speed and High is optimized for quality.
	// Params Lime.Enum.TextureCreationQuality quality
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

	// Sets the render quality of the scene using `Lime.Enum.Quality`.
	// Params Lime.Enum.Quality quality
	// Returns void
	module.set_function("setRenderQuality", &Module::Scene::Bind::SetRenderQuality);

	// Returns a `Mesh` containing a cube.
	// Params Vec3 size
	// Returns Mesh
	module.set_function("createCubeMesh", &Module::Scene::Bind::CreateCubeMesh);

	// Returns a `Mesh` containing a sphere.
	// Params number radius
	// Params number radius, number polyCount
	// Returns Mesh
	module.set_function("createSphereMesh", &Module::Scene::Bind::CreateSphereMesh);

	// Returns a `Mesh` containing a cylinder.
	// Params number radius, number length
	// Params number radius, number length, number polyCount, boolean closed
	// Returns Mesh
	module.set_function("createCylinderMesh", &Module::Scene::Bind::CreateCylinderMesh);

	// Returns a `Mesh` containing a plane. Parameter `repeatCount` controls how much an applied `Texture` will repeat within one tile.
	// Params Vec2 tileSize, Vec2 tileCount
	// Params Vec2 tileSize, Vec2 tileCount, Vec2 repeatCount
	// Returns Mesh
	module.set_function("createPlaneMesh",
		sol::overload(
			sol::resolve<Mesh(const Vec2&, const Vec2&)>(&Module::Scene::Bind::CreatePlaneMesh),
			sol::resolve<Mesh(const Vec2&, const Vec2&, const Vec2&)>(&Module::Scene::Bind::CreatePlaneMesh)
		));

	// If set to false, the application will not update the scene output. A use case is a pseudo-pause your game to save on resources while loading assets.
	// Params boolean active
	// Returns void
	module.set_function("setRenderingActive", &Module::Scene::Bind::SetActivelyRendering);

	// Returns whether or not the application is actively rendering new output from the scene.
	// Returns boolean
	module.set_function("isRenderingActive", &Module::Scene::Bind::GetActivelyRendering);

	// Fires a raycast out into the scene from `startPos` to `endPos`. Only objects with collision enabled will be tested.
	// Params Vec3 startPos, Vec3 endPos, number? rayLifeMs
	// Returns HitResult
	module.set_function("fireRaycast", &Module::Scene::Bind::FireRaycast);

	// End Module

	// Object HitResult, An object that stores raycast hit data.
	// Field Vec3 startPos, The starting position of this raycast.
	// Field Vec3 endPos, The ending position of this raycast. If an object was hit, this will be the hit position.
	// Field Vec3 normal, The normal vector of the hit triangle, if any.
	// Field number objectID, If hit, this will be the hit object's ID. Else, 0.
	// Field number materialID, If hit, this will be the hit material's ID. Else, 0.
	// Field boolean hit, True if the raycast hit a collidable object.
	// Field table attributes, Attributes of the hit object, if any.
	// End Object
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

void Module::Scene::Bind::SetRenderQuality(int q) {
	r->setSceneRenderQuality(q);
}

void Module::Scene::Bind::SetActivelyRendering(bool v) {
	r->setActivelyRendering(v);
}

bool Module::Scene::Bind::GetActivelyRendering() {
	return r->getIsActivelyRendering();
}

void Module::Scene::Bind::SetLightManagerType(int type) {
	r->setLightManagementType(type);
}

#include "Objects/HitResult.h"
sol::table Module::Scene::Bind::FireRaycast(const Vec3& start, const Vec3& end, float life) {
	HitResult hit = rh->fireRaycast(start, end, life);

	sol::state& lua = a->GetLuaState();
	sol::table out = lua.create_table();

	out["hit"] = hit.hit;
	out["startPos"] = hit.startPos;
	out["endPos"] = hit.endPos;
	out["normal"] = hit.normal;
	out["objID"] = hit.objID;
	out["matID"] = hit.matID;
	out["attr"] = hit.attr;

	return out;
}

Mesh Module::Scene::Bind::CreateCubeMesh(const Vec3& size) {
	return rh->createCubeMesh(size);
}

Mesh Module::Scene::Bind::CreateSphereMesh(float r, int polyCount) {
	return rh->createSphereMesh(r, polyCount);
}

Mesh Module::Scene::Bind::CreateCylinderMesh(float r, float l, int polyCount, bool closed) {
	return rh->createCylinderMesh(r, l, polyCount, closed);
}

Mesh Module::Scene::Bind::CreatePlaneMesh(const Vec2& tileSize, const Vec2& tileCount) {
	return rh->createPlaneMesh(tileSize, tileCount, tileCount);
}

Mesh Module::Scene::Bind::CreatePlaneMesh(const Vec2& tileSize, const Vec2& tileCount, const Vec2& texRepeat) {
	return rh->createPlaneMesh(tileSize, tileCount, texRepeat);
}
