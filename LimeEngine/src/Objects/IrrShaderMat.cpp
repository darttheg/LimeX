#include "Objects/IrrShaderMat.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"

#include "Renderer.h"
#include <iostream>

static Renderer* r;

IrrShaderMaterial::IrrShaderMaterial(irr::video::IVideoDriver* driver, const std::string& vsPath, const std::string& psPath, int type) {
	auto* gpu = driver->getGPUProgrammingServices();
	if (!gpu) return;

	this->type = gpu->addHighLevelShaderMaterialFromFiles(
		vsPath.c_str(), "vertexMain", irr::video::EVST_VS_3_0,
		psPath.c_str(), "pixelMain", irr::video::EPST_PS_3_0,
		this,
		static_cast<irr::video::E_MATERIAL_TYPE>(type),
		irr::video::EGSL_DEFAULT
	);
}

void IrrShaderMaterial::setRenderer(Renderer* r1) {
	r = r1;
}

void IrrShaderMaterial::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) {
	auto* driver = services->getVideoDriver();
	if (!driver) return;

	cachedServices = services;

	wvp = driver->getTransform(irr::video::ETS_PROJECTION);
	wvp *= driver->getTransform(irr::video::ETS_VIEW);
	wvp *= driver->getTransform(irr::video::ETS_WORLD);
	world = driver->getTransform(irr::video::ETS_WORLD);

	irr::core::matrix4 wv = driver->getTransform(irr::video::ETS_VIEW);
	wv *= driver->getTransform(irr::video::ETS_WORLD);

	const irr::f32 t = r ? r->getDtTime() : 0.0f;

	services->setVertexShaderConstant("mWorldView", wv.pointer(), 16);
	services->setVertexShaderConstant("mWorldViewProj", wvp.pointer(), 16);
	services->setVertexShaderConstant("mWorld", world.pointer(), 16);
	services->setVertexShaderConstant("uTime", &t, 1);

	const irr::f32 texLayer = 0;
	services->setPixelShaderConstant("screenTex", &texLayer, 1);

	for (auto& [name, val] : uniforms) {
		std::visit([&](auto&& v) {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, float>) {
				services->setVertexShaderConstant(name.c_str(), &v, 1);
				services->setPixelShaderConstant(name.c_str(), &v, 1);
			}
			else if constexpr (std::is_same_v<T, int>) {
				services->setVertexShaderConstant(name.c_str(), (irr::f32*)&v, 1);
				services->setPixelShaderConstant(name.c_str(), (irr::f32*)&v, 1);
			}
			else if constexpr (std::is_same_v<T, Vec2>) {
				irr::f32 buf[2] = { v.getX(), v.getY() };
				services->setVertexShaderConstant(name.c_str(), buf, 2);
				services->setPixelShaderConstant(name.c_str(), buf, 2);
			} else if constexpr (std::is_same_v<T, Vec3>) {
				irr::f32 buf[3] = { v.getX(), v.getY(), v.getZ() };
				services->setVertexShaderConstant(name.c_str(), buf, 3);
				services->setPixelShaderConstant(name.c_str(), buf, 3);
			} else if constexpr (std::is_same_v<T, Vec4>) {
				irr::f32 buf[4] = { v.getX(), v.getY(), v.getZ(), v.getW() };
				services->setVertexShaderConstant(name.c_str(), buf, 4);
				services->setPixelShaderConstant(name.c_str(), buf, 4);
			}
		}, val);
	}
}

void IrrShaderMaterial::setUniformFloat(const std::string& name, float v) {
	uniforms[name] = v;
}

void IrrShaderMaterial::setUniformInt(const std::string& name, int v) {
	uniforms[name] = v;
}

void IrrShaderMaterial::setUniformVec2(const std::string& name, const Vec2& v) {
	uniforms[name] = v;
}

void IrrShaderMaterial::setUniformVec3(const std::string& name, const Vec3& v) {
	uniforms[name] = v;
}

void IrrShaderMaterial::setUniformVec4(const std::string& name, const Vec4& v) {
	uniforms[name] = v;
}