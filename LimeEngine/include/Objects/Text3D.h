#pragma once
#include "Interfaces/SceneObject.h"
#include "Interfaces/Object3D.h"

extern "C" { struct lua_State; }
class Texture;
class Material;
class Vec2;
class Vec3;

class RenderHelper;
class GUIManager;

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
	bool getWordWrap();
	void setWordWrap(bool enable);

	Vec2 getSize() const;
	void setSize(const Vec2& size);

	bool setFont(const std::string& f);

    irr::scene::ISceneNode* getNode() const override;
private:
	bool wraps = false;
    irr::scene::CTextAnchorSceneNode* wrap = nullptr;
	irr::gui::CGUIColoredText* src = nullptr;
};

namespace Object::Text3DBind {
    void bind(lua_State* ls, RenderHelper* renh, GUIManager* gu);
}