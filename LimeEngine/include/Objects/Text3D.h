#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

class Application;
class Texture;
class Material;
class Vec2;
class Vec3;

namespace irr {
    namespace scene {
        class ISceneNode;
        class IBillboardSceneNode;
        class CTextAnchorSceneNode;
    }
	namespace gui {
		class CGUIColoredText;
	}
}

class Text3D : public SceneObject, public Object3D {
public:
	Text3D();
	Text3D(const std::string& tx);
	void destroy() override;
	void setDebug(bool v) override;

	std::string getText();
	void setText(const std::string& tx);

	int getOpacity();
	void setOpacity(int o);
	void setAlignment(int all);
	void setAlignment(int h, int v);
	void setWordWrap(bool enable);

	Vec2 getSize() const;
	void setSize(const Vec2& size);

	bool setFont(const std::string& f);

    irr::scene::ISceneNode* getNode() const override;
private:
    irr::scene::CTextAnchorSceneNode* wrap = nullptr;
	irr::gui::CGUIColoredText* src = nullptr;
};

namespace Object::Text3DBind {
    void bind(Application* app);
}