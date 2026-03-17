#pragma once

#include <string>
#include <sol/forward.hpp>

class DebugConsole;
class Vec4;
class Vec3;
class Vec2;
class Mesh;

namespace irr {
	class IrrlichtDevice;
	namespace scene {
		class ISceneManager;
		class ISceneNode;
		class ICameraSceneNode;
		class IBillboardSceneNode;
		class CTextAnchorSceneNode;
		class IAnimatedMeshSceneNode;
		class IOctreeSceneNode;
		class IMesh;
		class IAnimatedMesh;
		class ITriangleSelector;
		class IMeshSceneNode;
		class ISceneCollisionManager;
	}
	namespace video {
		class IVideoDriver;
		class IGPUProgrammingServices;
		class ITexture;
	}
	namespace gui {
		class IGUIEnvironment;
		class CGUIColoredText;
		class IGUIStaticText;
		class IGUIImage;
		class IGUIButton;
		class IGUIElement;
	}
}
using namespace irr;

enum DEBUG3D_TYPE {
	MESH = 0,
	CAMERA,
	LIGHT,
	TEXT,
	SKYDOME,
	EMPTY,
	BILLBOARD
};

class RenderHelper {
public:
	RenderHelper() = default;
	~RenderHelper() = default;
	bool guardRenderingCheck(std::string msg = "");

	void Init(irr::IrrlichtDevice* device, DebugConsole* debug);

	irr::scene::IBillboardSceneNode* createDebugNode(DEBUG3D_TYPE t);

	irr::video::ITexture* createTexture(int w, int h, const std::string& name = "");
	irr::video::ITexture* createTexture(const std::string& path);
	irr::video::ITexture* cropTexture(irr::video::ITexture* tex, const Vec2& pos, const Vec2& dim);
	irr::video::ITexture* appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos);
	Vec4 getColor(irr::video::ITexture* tex, const Vec2& pos);
	irr::video::ITexture* setColor(irr::video::ITexture* tex, const Vec2& pos, const Vec4& color);
	void keyColor(irr::video::ITexture* tex, const Vec4& color);
	void setVertexColor(irr::scene::IAnimatedMeshSceneNode* m, const Vec4& color);

	// Create nodes
	irr::scene::IAnimatedMesh* createMesh(const std::string& path);
	irr::scene::IAnimatedMeshSceneNode* createEmptyMesh();
	irr::scene::IAnimatedMeshSceneNode* createAnimatedMesh(irr::scene::IAnimatedMesh* m);
	irr::scene::IMeshSceneNode* createOctreeMesh(irr::scene::IAnimatedMeshSceneNode* m);
	irr::scene::ICameraSceneNode* createCameraNode();
	irr::scene::ISceneNode* createEmptyNode();
	irr::scene::ISceneNode* createSkydomeNode(irr::video::ITexture* tex);
	irr::scene::IBillboardSceneNode* createBillboardNode();
	irr::scene::CTextAnchorSceneNode* createText3DNode(irr::gui::CGUIColoredText* src);
	irr::scene::ITriangleSelector* createTriangleSelector(irr::scene::IAnimatedMeshSceneNode* m);
	Mesh createCubeMesh(const Vec3& size);
	Mesh createSphereMesh(float r, int polyCount);
	Mesh createCylinderMesh(float r, float l, int polyCount, bool closed = true);
	Mesh createPlaneMesh(const Vec2& tileSize, const Vec2& tileCount, const Vec2& texRepeat);
	sol::table fireRaycast(const Vec3& start, const Vec3& end, float life);

	void updateCameraMatrix(irr::scene::ICameraSceneNode* c);
	void setActiveCamera(irr::scene::ICameraSceneNode* c);

	// GUI
	irr::gui::CGUIColoredText* createColoredText2D();
	irr::gui::IGUIStaticText* createStaticText();
	irr::gui::IGUIImage* createGUIImage();
	irr::gui::IGUIButton* createButton();
	irr::gui::IGUIElement* getGUIRoot();

private:
	irr::IrrlichtDevice* i_device = nullptr;
	irr::scene::ISceneManager* i_smgr = nullptr;
	irr::video::IVideoDriver* i_driver = nullptr;
	irr::gui::IGUIEnvironment* i_gui = nullptr;
	irr::video::IGPUProgrammingServices* i_gpu = nullptr;
};