#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include "irrlicht.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

class Renderer;

using UniformValue = std::variant<float, int, Vec2, Vec3, Vec4>;

class IrrShaderMaterial : public irr::video::IShaderConstantSetCallBack {
public:
	IrrShaderMaterial(irr::video::IVideoDriver* driver, const std::string& vsPath, const std::string& psPath, int type);

	void setRenderer(Renderer* r);
	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) override;
	void setUniformFloat(const std::string& name, float v);
	void setUniformInt(const std::string& name, int v);
	void setUniformVec2(const std::string& name, const Vec2& v);
	void setUniformVec3(const std::string& name, const Vec3& v);
	void setUniformVec4(const std::string& name, const Vec4& v);

	irr::s32 getMaterialType() const { return type; }
private:
	irr::s32 type = -1;
	std::unordered_map<std::string, UniformValue> uniforms;
	irr::core::matrix4 wvp;
	irr::core::matrix4 world;
};