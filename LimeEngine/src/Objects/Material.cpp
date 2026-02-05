#include "Objects/Material.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"

#include <algorithm>
#include "irrlicht.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Image.h"

static DebugConsole* d;
static Renderer* r;

Material::Material() {
	material = std::make_unique<irr::video::SMaterial>();
	setQuality(1);
}

Material::Material(const Image& img) {
	material = std::make_unique<irr::video::SMaterial>();
	loadImage(img);
	setQuality(1);
}

Material::Material(const Material& other) {
	material = std::make_unique<irr::video::SMaterial>(*other.material);
}

Material::Material(int quality) {
	material = std::make_unique<irr::video::SMaterial>();
	setQuality(quality);
}

int Material::getID() const {
	return material->ID;
}

void Material::setID(int v) {
	material->ID = v;
}

Vec2 Material::getImageScroll(int layer) const {
	if (layer < 0) layer = 0;
	auto& m = material->getTextureMatrix(layer);
	return Vec2(m.getTranslation().X, m.getTranslation().Y);
}

void Material::setImageScroll(int layer, const Vec2& coords) {
	if (layer < 0) layer = 0;
	auto& m = material->getTextureMatrix(layer);
	m.setTextureTranslate(coords.getX(), coords.getY());
	material->setTextureMatrix(layer, m);
}

void Material::setImageScroll(const Vec2& coords) {
	setImageScroll(0, coords);
}

void Material::loadImage(int layer, const Image& img) {
	if (layer < 0) layer = 0;
	material->setTexture(layer, img.getTexture());
}

void Material::loadImage(const Image& img) {
	loadImage(0, img);
}

void Material::clearImage(int layer) {
	if (layer < 0) layer = 0;
	material->setTexture(layer, nullptr);
}

Vec2 Material::getImageUVWrapBehavior(int layer) const {
	if (layer < 0) layer = 0;
	return Vec2(material->TextureLayer[layer].TextureWrapU, material->TextureLayer[layer].TextureWrapV);
}

void Material::setImageUVWrapBehavior(int layer, int u, int v) {
	if (layer < 0) layer = 0;
	material->TextureLayer[layer].TextureWrapU = u;
	material->TextureLayer[layer].TextureWrapV = v;
}

void Material::setImageUVWrapBehavior(int u, int v) {
	setImageUVWrapBehavior(0, u, v);
}

Vec2 Material::getImageScale(int layer) const {
	if (layer < 0) layer = 0;
	auto& m = material->getTextureMatrix(layer);
	return Vec2(m.getScale().X, m.getScale().Y);
}

void Material::setImageScale(int layer, const Vec2& scale) {
	if (layer < 0) layer = 0;
	auto& m = material->getTextureMatrix(layer);
	m.setTextureScale(scale.getX(), scale.getY());
	material->setTextureMatrix(layer, m);
}

void Material::setImageScale(const Vec2& scale) {
	setImageScale(0, scale);
}

int Material::getType() const {
	return material->MaterialType;
}

void Material::setType(int v) {
	material->MaterialType = static_cast<irr::video::E_MATERIAL_TYPE>(v);
}

bool Material::getFog() const {
	return material->FogEnable;
}

void Material::setFog(bool on) {
	material->FogEnable = on;
}

bool Material::getLighting() const {
	return material->Lighting;
}

void Material::setLighting(bool on) {
	material->Lighting = on;
}

bool Material::getBFCulling() const {
	return material->BackfaceCulling;
}

void Material::setBFCulling(bool on) {
	material->BackfaceCulling = on;
}

bool Material::getFFCulling() const {
	return material->FrontfaceCulling;
}

void Material::setFFCulling(bool on) {
	material->FrontfaceCulling = on;
}

int Material::getQuality() const {
	return m_quality;
}

void Material::setQuality(int quality) {
	if (m_quality == quality) return;
	m_quality = quality;

	auto setFilters = [&](bool bilinear, bool trilinear, u32 aniso) {
			for (int i = 0; i < 2; i++) {
				material->TextureLayer[i].AnisotropicFilter = aniso;
				material->TextureLayer[i].BilinearFilter = bilinear;
				material->TextureLayer[i].TrilinearFilter = trilinear;
			}
		};

	switch (quality) {
	case 1: // Medium
		material->AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_SIMPLE;
		setFilters(true, false, 0);
		break;
	case 2: // High
		material->AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_QUALITY;
		setFilters(true, true, 0);
		break;
	case 3: // Ultra
		material->AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_ALPHA_TO_COVERAGE;
		setFilters(true, true, 16);
		break;
	default: // Low
		material->AntiAliasing = irr::video::E_ANTI_ALIASING_MODE::EAAM_OFF;
		setFilters(false, false, 0);
		break;
	}
}

bool Material::getWireframe() const {
	return material->Wireframe;
}

void Material::setWireframe(bool on) {
	material->Wireframe = on;
}

int Material::getZOrdering() const {
	return static_cast<int>(material->ZBuffer);
}

void Material::setZOrdering(int method) {
	material->ZBuffer = static_cast<irr::video::E_COMPARISON_FUNC>(method);
}

bool Material::getWriteToDepth() const {
	return material->ZWriteEnable;
}

void Material::setWriteToDepth(bool on) {
	material->ZWriteEnable = on;
}

float Material::getOpacity() const {
	return static_cast<float>(material->DiffuseColor.getAlpha()) / 255.0f;
}

void Material::setOpacity(float v) {
	v = std::clamp(v, 0.0f, 1.0f);
	const irr::u32 outA = static_cast<irr::u32>(v * 255.0f + 0.5f);

	material->AmbientColor.setAlpha(outA);
	material->DiffuseColor.setAlpha(outA);
}

bool Material::getMipMaps() const {
	return material->UseMipMaps;
}

void Material::setMipMaps(bool on) {
	material->UseMipMaps = on;
}

float Material::getShine() const {
	return static_cast<float>(material->Shininess) / 30.0f;
}

void Material::setShine(float v) {
	v = std::clamp(v, 0.0f, 1.0f);
	material->Shininess = static_cast<irr::f32>(v * 30.0f);
}

Vec4 Material::getAmbientColor() const {
	irr::video::SColor c = material->AmbientColor;
	return Vec4(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
}

Vec4 Material::getDiffuseColor() const {
	irr::video::SColor c = material->DiffuseColor;
	return Vec4(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
}

Vec4 Material::getSpecularColor() const {
	irr::video::SColor c = material->SpecularColor;
	return Vec4(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
}

Vec4 Material::getEmissiveColor() const {
	irr::video::SColor c = material->EmissiveColor;
	return Vec4(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
}

void Material::setAmbientColor(const Vec4& rgba) {
	irr::video::SColor c = irr::video::SColor(rgba.getW(), rgba.getX(), rgba.getY(), rgba.getZ());
	material->AmbientColor = c;
}

void Material::setDiffuseColor(const Vec4& rgba) {
	irr::video::SColor c = irr::video::SColor(rgba.getW(), rgba.getX(), rgba.getY(), rgba.getZ());
	material->DiffuseColor = c;
}

void Material::setSpecularColor(const Vec4& rgba) {
	irr::video::SColor c = irr::video::SColor(rgba.getW(), rgba.getX(), rgba.getY(), rgba.getZ());
	material->SpecularColor = c;
}

void Material::setEmissiveColor(const Vec4& rgba) {
	irr::video::SColor c = irr::video::SColor(rgba.getW(), rgba.getX(), rgba.getY(), rgba.getZ());
	material->EmissiveColor = c;
}

void Object::MaterialBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	// Object Material, An object used to hold material parameters for 3D objects. A Material has at most two layers, with Lime.MaterialType allowing for different combinations of said layers.

	// Constructor
	// Constructor Image img
	// Constructor Material other
	// Constructor Lime.MaterialQuality quality

	sol::state_view view(a->GetLuaState());
	sol::usertype<Material> obj = view.new_usertype<Material>(
		"Material",
		sol::constructors<Material(), Material(const Image& img), Material(const Material& other), Material(int quality)>(),
		sol::meta_function::type, [](const Material&) { return "Material"; },

		// Field number ID, An ID to identify this Material with, being useful for raycast hit results as those can contain a hit Material ID.
		"ID", sol::property(&Material::getID, &Material::setID),
		// Field Lime.MaterialType type, Sets the type of this Material, determing how the layers interact with themselves and the world
		"type", sol::property(&Material::getType, &Material::setType),
		// Field boolean fog, Enables fog for this Material
		"fog", sol::property(&Material::getFog, &Material::setFog),
		// Field boolean lighting, Enables lighting for this Material
		"lighting", sol::property(&Material::getLighting, &Material::setLighting),
		// Field boolean backfaceCulling, Change backface culling behavior for this Material
		"backfaceCulling", sol::property(&Material::getBFCulling, &Material::setBFCulling),
		// Field boolean frontfaceCulling, Change frontface culling behavior for this Material
		"frontfaceCulling", sol::property(&Material::getFFCulling, &Material::setFFCulling),
		// Field Lime.MaterialQuality quality, Sets the quality of this Material using Lime.MaterialQuality presets, where Low is retro/old-school and Ultra is smooth and high quality
		"quality", sol::property(&Material::getQuality, &Material::setQuality),
		// Field boolean wireframe, Enables wireframe view for this Material
		"wireframe", sol::property(&Material::getWireframe, &Material::setWireframe),
		// Field Lime.ZOrderMethod zMethod, Sets Z ordering method for this Material using Lime.ZOrderMethod
		"zMethod", sol::property(&Material::getZOrdering, &Material::setZOrdering),
		// Field number opacity, Sets the opacity of this Material from 0.0 (invisible) to 1.0 (visible), affecting the transparency of objects with this Material applied (NOTE: Will not affect solid Materials)
		"opacity", sol::property(&Material::getOpacity, &Material::setOpacity),
		// Field boolean mipmaps, Enables the generation of mipmaps
		"mipmaps", sol::property(&Material::getMipMaps, &Material::setMipMaps),
		// Field number shine, Sets the shine for this Material, ranging from 0 (soft and wide shine) to 1 (harsh and small shine)
		"shine", sol::property(&Material::getShine, &Material::setShine),
		// Field boolean writeToDepth, Enables this material writing to the depth buffer on render, where false is common for transparent objects
		"writeToDepth", sol::property(&Material::getWriteToDepth, &Material::setWriteToDepth),

		// Field Vec4 ambientColor, Sets the ambient color for this Material, the base color
		"ambientColor", sol::property(
			[](Material& c) { return Vec4{ [&] { return c.getAmbientColor(); }, [&](auto v) { c.setAmbientColor(v); } }; },
			[](Material& c, const Vec4& v) { c.setAmbientColor(v); }
		),

		// Field Vec4 diffuseColor, Sets the diffuse color for this Material, the light-affected base color
		"diffuseColor", sol::property(
			[](Material& c) { return Vec4{ [&] { return c.getDiffuseColor(); }, [&](auto v) { c.setDiffuseColor(v); } }; },
			[](Material& c, const Vec4& v) { c.setDiffuseColor(v); }
		),

		// Field Vec4 specularColor, Sets the specular color for this Material, the shine color
		"specularColor", sol::property(
			[](Material& c) { return Vec4{ [&] { return c.getSpecularColor(); }, [&](auto v) { c.setSpecularColor(v); } }; },
			[](Material& c, const Vec4& v) { c.setSpecularColor(v); }
		),

		// Field Vec4 emissiveColor, Sets the emissive color for this Material, the color that is seen through shadows, lighting, and fog
		"emissiveColor", sol::property(
			[](Material& c) { return Vec4{ [&] { return c.getEmissiveColor(); }, [&](auto v) { c.setEmissiveColor(v); } }; },
			[](Material& c, const Vec4& v) { c.setEmissiveColor(v); }
		)
	);

	obj[sol::meta_function::to_string] = [](const Material& v) {
		return "Material";
		};

	// Loads an Image into this Material
	// Params number layer, Image img
	// Params Image img
	// Returns void
	obj.set_function("loadImage",
		sol::overload(
			sol::resolve<void(int, const Image&)>(&Material::loadImage),
			sol::resolve<void(const Image&)>(&Material::loadImage)
		));

	// Clears the Image in this Material
	// Params number? layer
	// Returns void
	obj.set_function("clearImage", &Material::clearImage);

	// Changes the method for Image UV wrapping
	// Params number layer, Lime.ImageWrapType uMethod, Lime.ImageWrapType vMethod
	// Params Lime.ImageWrapType uMethod, Lime.ImageWrapType vMethod
	// Returns void
	obj.set_function("setImageWrapMethod",
		sol::overload(
			sol::resolve<void(int, int, int)>(&Material::setImageUVWrapBehavior),
			sol::resolve<void(int, int)>(&Material::setImageUVWrapBehavior)
		));

	// Sets the scale of an Image's mapping
	// Params number layer, Vec2 scale
	// Params Vec2 scale
	// Returns void
	obj.set_function("setImageScale",
		sol::overload(
			sol::resolve<void(int, const Vec2&)>(&Material::setImageScale),
			sol::resolve<void(const Vec2&)>(&Material::setImageScale)
		));

	// End Object
}