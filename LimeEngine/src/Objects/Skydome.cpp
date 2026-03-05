#include "Objects/Skydome.h"

#include "Application.h"
#include "RenderHelper.h"
#include "DebugConsole.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

#include "irrlicht.h"

static DebugConsole* d;
static RenderHelper* rh;

Skydome::Skydome() {
	sky = rh->createSkydomeNode(nullptr);
}

Skydome::Skydome(const Material& mat) {
	sky = rh->createSkydomeNode(nullptr);
	loadMaterial(mat);
}

void Skydome::destroy() {
	if (sky) sky->remove();
	sky = nullptr;
}

void Skydome::setDebug(bool v) {
}

using namespace irr;
using namespace video;
void Skydome::loadMaterial(const Material& mat) {
	if (!sky) return;

	sky->setMaterialTexture(0, mat.getMaterial().getTexture(0));
	sky->setMaterialType(mat.getMaterial().MaterialType);
	sky->setMaterialFlag(E_MATERIAL_FLAG::EMF_ANTI_ALIASING, mat.getMaterial().getFlag(E_MATERIAL_FLAG::EMF_ANTI_ALIASING));
	sky->setMaterialFlag(E_MATERIAL_FLAG::EMF_ANISOTROPIC_FILTER, mat.getMaterial().getFlag(E_MATERIAL_FLAG::EMF_ANISOTROPIC_FILTER));
	sky->setMaterialFlag(E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, mat.getMaterial().getFlag(E_MATERIAL_FLAG::EMF_BILINEAR_FILTER));
	sky->setMaterialFlag(E_MATERIAL_FLAG::EMF_TRILINEAR_FILTER, mat.getMaterial().getFlag(E_MATERIAL_FLAG::EMF_TRILINEAR_FILTER));
	sky->setMaterialFlag(E_MATERIAL_FLAG::EMF_TEXTURE_WRAP, mat.getMaterial().getFlag(E_MATERIAL_FLAG::EMF_TEXTURE_WRAP));
	sky->setMaterialFlag(E_MATERIAL_FLAG::EMF_USE_MIP_MAPS, mat.getMaterial().getFlag(E_MATERIAL_FLAG::EMF_USE_MIP_MAPS));
}

void Object::SkydomeBind::bind(Application* a) {
	rh = a->GetRenderHelper();
	d = a->GetDebugConsole();

	sol::state_view view(a->GetLuaState());
	sol::usertype<Skydome> obj = view.new_usertype<Skydome>(
		"Skydome",
		sol::constructors<Skydome(), Skydome(const Material& material)>(),

		sol::base_classes, sol::bases<Object3D>(),
		sol::meta_function::type, [](const Skydome&) { return "Skydome"; }
	);

	obj[sol::meta_function::to_string] = [](const Skydome& v) {
		return "Skydome";
		};

	// Object Skydome, A dome rendered behind all scene objects, like a sky.
	// Inherits Object3D

	// Constructor
	// Constructor Material material

	// Loads a new Material into this Skydome
	// Params Material material
	// Returns void
	obj.set_function("loadMaterial", &Skydome::loadMaterial);

	// End Object
}