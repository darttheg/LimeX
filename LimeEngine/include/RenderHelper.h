#pragma once

#include <string>
#include <sol/forward.hpp>
#include <unordered_map>

class DebugConsole;
class Vec4;
class Vec3;
class Vec2;
class Mesh;
class HitResult;

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
		class ILightSceneNode;
		class IParticleSystemSceneNode;
		class ISceneNodeAnimator;
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
		class IGUIEditBox;
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
	BILLBOARD,
	SOUND,
	PARTICLES
};

class RenderHelper {
public:
	RenderHelper(DebugConsole* debug);
	~RenderHelper() = default;
	bool guardRenderingCheck(std::string msg = "");

	void Init(irr::IrrlichtDevice* device);
	void SetLuaState(sol::state* s);

	irr::scene::IBillboardSceneNode* createDebugNode(DEBUG3D_TYPE t);

	irr::video::ITexture* createTexture(int w, int h, const std::string& name = "");
	irr::video::ITexture* createTexture(const std::string& path);
	irr::video::ITexture* cropTexture(irr::video::ITexture* tex, const Vec2& pos, const Vec2& dim);
	irr::video::ITexture* appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos);
	Vec4 getColor(irr::video::ITexture* tex, const Vec2& pos);
	bool setColor(irr::video::ITexture* tex, const Vec2& pos, const Vec4& color);
	bool setColorRect(irr::video::ITexture* tex, const Vec2& tl, const Vec2& br, const Vec4& color);
	bool keyColor(irr::video::ITexture* tex, const Vec4& color);
	void clearTextureWithColor(irr::video::ITexture* tex, const Vec4& color);
	bool setVertexColor(irr::scene::IAnimatedMeshSceneNode* m, const Vec4& color);

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
	irr::scene::ILightSceneNode* createLight();
	irr::scene::IParticleSystemSceneNode* createParticleSystem();
	irr::scene::ITriangleSelector* createTriangleSelector(irr::scene::IAnimatedMeshSceneNode* m);
	Mesh createCubeMesh(const Vec3& size);
	Mesh createSphereMesh(float r, int polyCount);
	Mesh createCylinderMesh(float r, float l, int polyCount, bool closed = true);
	Mesh createPlaneMesh(const Vec2& tileSize, const Vec2& tileCount, const Vec2& texRepeat);
	HitResult fireRaycast(const Vec3& start, const Vec3& end, float life);
	HitResult fireScreenRaycast(const Vec2& start, float len, float life);

	// Misc
	Vec2 toScreenPos(const Vec3& pos);
	int getCurrentTime();

	// Animators
	irr::scene::ISceneNodeAnimator* createDestroyAnimator(int ms);
	irr::scene::ISceneNodeAnimator* createFlyStraightAnimator(const Vec3& start, const Vec3& end, int ms, bool loops = false, bool pingPong = false);
	irr::scene::ISceneNodeAnimator* createRotationAnimator(const Vec3& rot);

	// Camera
	void updateCameraMatrix(irr::scene::ICameraSceneNode* c);
	void setActiveCamera(irr::scene::ICameraSceneNode* c, irr::scene::ISceneNode* l, irr::scene::ISceneNode* f);
	irr::scene::ICameraSceneNode* getActiveCamera();
	irr::scene::ISceneNode* getActiveCameraLeft() { return cleft; }
	irr::scene::ISceneNode* getActiveCameraForward() { return cforward; }

	// GUI
	irr::gui::CGUIColoredText* createColoredText2D();
	irr::gui::IGUIStaticText* createStaticText();
	irr::gui::IGUIImage* createGUIImage();
	irr::gui::IGUIButton* createButton();
	irr::gui::IGUIElement* getGUIRoot();
	irr::gui::IGUIEditBox* createEditBox();

	// Attributes
	sol::object getAttribute(irr::scene::ISceneNode* node, sol::object key);
	void setAttribute(irr::scene::ISceneNode* node, sol::object key, sol::object value);
	sol::table getAttributes(irr::scene::ISceneNode* node);
	void clearAttributes(irr::scene::ISceneNode* node);
private:
	irr::IrrlichtDevice* i_device = nullptr;
	irr::scene::ISceneManager* i_smgr = nullptr;
	irr::video::IVideoDriver* i_driver = nullptr;
	irr::gui::IGUIEnvironment* i_gui = nullptr;
	irr::video::IGPUProgrammingServices* i_gpu = nullptr;

	irr::scene::ISceneNode* cleft = nullptr;
	irr::scene::ISceneNode* cforward = nullptr;

	std::unordered_map<irr::scene::ISceneNode*, sol::table> attributes;

	int customTexCount = 0;
};