#pragma once
#include <string>
#include <memory>

extern "C" { struct lua_State; }

class IrrShaderMaterial;
class Vec2;
class Vec3;
class Vec4;
class Renderer;
class Object3D;

class ShaderMaterial {
public:
	ShaderMaterial(const std::string& vsPath, const std::string& psPath, int type = 0);
	ShaderMaterial(const std::string& hlsl, int type = 0);
	~ShaderMaterial();

	void setUniformFloat(const std::string& name, float v);
	void setUniformInt(const std::string& name, int v);
	void setUniformVec2(const std::string& name, const Vec2& v);
	void setUniformVec3(const std::string& name, const Vec3& v);
	void setUniformVec4(const std::string& name, const Vec4& v);
	void setUniformMat4(const std::string& name, const Object3D& obj);
	void setUniformMat4(const std::string& name, const Object3D& obj, bool inverse);

	int getMaterialType() const;
	bool isValid() const { return shadermat; }
	IrrShaderMaterial* getInternalMat() const { return shadermat; }

	std::string getVsPath() const;
	std::string getPsPath() const;
private:
	IrrShaderMaterial* shadermat;
	std::string vsPath;
	std::string psPath;
};

namespace Object::ShaderMaterialBind {
	void bind(lua_State* ls, Renderer* rend);
}