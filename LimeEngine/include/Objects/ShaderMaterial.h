#pragma once
#include <string>
#include <memory>

class Application;

class IrrShaderMaterial;
class Vec2;
class Vec3;
class Vec4;

class ShaderMaterial {
public:
	ShaderMaterial(const std::string& vsPath, const std::string& psPath, int type = 0);

	void setUniformFloat(const std::string& name, float v);
	void setUniformInt(const std::string& name, int v);
	void setUniformVec2(const std::string& name, const Vec2& v);
	void setUniformVec3(const std::string& name, const Vec3& v);
	void setUniformVec4(const std::string& name, const Vec4& v);

	int getMaterialType() const;
	bool isValid() const { return shadermat.get(); }

	std::string getVsPath() const;
	std::string getPsPath() const;
private:
	std::shared_ptr<IrrShaderMaterial> shadermat;
	std::string vsPath;
	std::string psPath;
};

namespace Object::ShaderMaterialBind {
	void bind(Application* app);
}