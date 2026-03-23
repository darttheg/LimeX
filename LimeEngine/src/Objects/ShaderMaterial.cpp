#include "Objects/ShaderMaterial.h"

#include "Application.h"
#include "DebugConsole.h"
#include "Renderer.h"

static DebugConsole* d;
static Renderer* r;

#include "Objects/IrrShaderMat.h"

ShaderMaterial::ShaderMaterial(const std::string& vs, const std::string& ps, int type) {
	shadermat = std::make_shared<IrrShaderMaterial>(r->getVideoDriver(), vs, ps, type);
	auto* fs = r->getFileSystem();
	if (!fs) return;
	vsPath = fs->getAbsolutePath(vs.c_str()).c_str();
	psPath = fs->getAbsolutePath(ps.c_str()).c_str();
}

void ShaderMaterial::setUniformFloat(const std::string& name, float v) {
	if (!shadermat) return;
	shadermat->setUniformFloat(name, v);
}

void ShaderMaterial::setUniformInt(const std::string& name, int v) {
	if (!shadermat) return;
	shadermat->setUniformInt(name, v);
}

void ShaderMaterial::setUniformVec2(const std::string& name, const Vec2& v) {
	if (!shadermat) return;
	shadermat->setUniformVec2(name, v);
}

void ShaderMaterial::setUniformVec3(const std::string& name, const Vec3& v) {
	if (!shadermat) return;
	shadermat->setUniformVec3(name, v);
}

void ShaderMaterial::setUniformVec4(const std::string& name, const Vec4& v) {
	if (!shadermat) return;
	shadermat->setUniformVec4(name, v);
}

int ShaderMaterial::getMaterialType() const {
	if (!shadermat) return irr::video::EMT_SOLID;
	return shadermat->getMaterialType();
}

std::string ShaderMaterial::getVsPath() const {
	if (!shadermat) return "";
	return vsPath;
}

std::string ShaderMaterial::getPsPath() const {
	if (!shadermat) return "";
	return psPath;
}

void Object::ShaderMaterialBind::bind(Application* a) {
	r = a->GetRenderer();
	d = a->GetDebugConsole();

	// Object ShaderMaterial, A special material that can produce custom effects. This object is applied to a `Material`, not directly to another 3D object. By default, all `ShaderMaterial` objects set internal parameters `uWVP` to the current world-view projection matrix and `uWorld` to just the current world matrix.

	// Constructor string vertexShaderPath, string pixelShaderPath, Lime.Enum.MaterialType? type

	sol::state_view view(a->GetLuaState());
	sol::usertype<ShaderMaterial> obj = view.new_usertype<ShaderMaterial>(
		"ShaderMaterial",
		sol::constructors<ShaderMaterial(const std::string& vsPath, const std::string& psPath), ShaderMaterial(const std::string &vsPath, const std::string &psPath, int type)>(),
		sol::meta_function::type, [](const ShaderMaterial&) { return "ShaderMaterial"; }
	);

	obj[sol::meta_function::to_string] = [](const ShaderMaterial& v) {
		return "ShaderMaterial";
		};

	// Sets a uniform shader parameter within this `ShaderMaterial`.
	// Params string name, number value
	// Params string name, Vec2 value
	// Params string name, Vec3 value
	// Params string name, Vec4 value
	// Returns void
	obj.set_function("setParameter",
		sol::overload(
			sol::resolve<void(const std::string&, int)>(&ShaderMaterial::setUniformInt),
			sol::resolve<void(const std::string&, float)>(&ShaderMaterial::setUniformFloat),
			sol::resolve<void(const std::string&, const Vec2&)>(&ShaderMaterial::setUniformVec2),
			sol::resolve<void(const std::string&, const Vec3&)>(&ShaderMaterial::setUniformVec3),
			sol::resolve<void(const std::string&, const Vec4&)>(&ShaderMaterial::setUniformVec4)
		));

	// Returns the path to the vertex shader file loaded in this `ShaderMaterial`.
	// Returns string
	obj.set_function("getVSPath", &ShaderMaterial::getVsPath);

	// Returns the path to the pixel shader file loaded in this `ShaderMaterial`.
	// Returns string
	obj.set_function("getPSPath", &ShaderMaterial::getPsPath);

	// End Object
}