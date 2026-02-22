#pragma once
#include <string>
#include <memory>

class Application;

class Vec2;
class Vec4;
class Texture;

namespace irr {
	namespace video {
		class SMaterial;
	}
}

class Material {
public:
	Material();
	Material(const Texture& img);
	Material(const Material& other);
	Material(int quality);

	int getID() const;
	void setID(int v);

	Vec2 getTextureScroll(int layer = 0) const;
	void setTextureScroll(int layer, const Vec2& coords);
	void setTextureScroll(const Vec2& coords);

	void loadTexture(int layer, const Texture& img);
	void loadTexture(const Texture& img);
	void clearTexture(int layer = 0);

	Vec2 getTextureUVWrapBehavior(int layer = 0) const;
	void setTextureUVWrapBehavior(int layer, int u, int v);
	void setTextureUVWrapBehavior(int u, int v);

	Vec2 getTextureScale(int layer = 0) const;
	void setTextureScale(int layer, const Vec2& scale);
	void setTextureScale(const Vec2& scale);

	int getType() const;
	void setType(int v);

	bool getFog() const;
	void setFog(bool on = true);

	bool getLighting() const;
	void setLighting(bool on = true);

	bool getBFCulling() const;
	void setBFCulling(bool on);

	bool getFFCulling() const;
	void setFFCulling(bool on);

	int getQuality() const;
	void setQuality(int quality); // Enum, combines AA and filtering into one function

	bool getWireframe() const;
	void setWireframe(bool on = true);

	int getZOrdering() const;
	void setZOrdering(int method); // Enum, determines z order comparison method, combine ZWrite into this too

	bool getWriteToDepth() const;
	void setWriteToDepth(bool on);

	Vec4 getAmbientColor() const;
	Vec4 getDiffuseColor() const;
	Vec4 getSpecularColor() const;
	Vec4 getEmissiveColor() const;
	void setAmbientColor(const Vec4& rgba);
	void setDiffuseColor(const Vec4& rgba);
	void setSpecularColor(const Vec4& rgba);
	void setEmissiveColor(const Vec4& rgba);

	float getOpacity() const;
	void setOpacity(float v);

	bool getMipMaps() const;
	void setMipMaps(bool on = true);

	float getShine() const;
	void setShine(float v);

	irr::video::SMaterial& getMaterial() const { return *material; }
private:
	std::unique_ptr<irr::video::SMaterial> material;
	int m_quality = -1;
};

namespace Object::MaterialBind {
	void bind(Application* app);
}