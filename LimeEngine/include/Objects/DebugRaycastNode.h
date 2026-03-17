#pragma once
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class RaycastVisualSceneNode : public ISceneNode {
private:
	aabbox3df bBox;
	u32 lastTime = 0;
public:
	vector3df start;
	vector3df end;
	bool hit = false;
	f32 life = 0.0f;

	RaycastVisualSceneNode(ISceneNode* parent, ISceneManager* smgr, s32 id = -1) : ISceneNode(parent, smgr, id) {}

	virtual void OnRegisterSceneNode() override {
		if (IsVisible) SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);
		ISceneNode::OnRegisterSceneNode();
	}

	virtual void OnAnimate(u32 timeMs) override {
		if (lastTime != 0) {
			life -= (f32)(timeMs - lastTime);
			if (life <= 0.0f) {
				remove();
				return;
			}
		}

		lastTime = timeMs;
		ISceneNode::OnAnimate(timeMs);
	}

	virtual void render() override {
		IVideoDriver* driver = SceneManager->getVideoDriver();
		driver->setTransform(ETS_WORLD, AbsoluteTransformation);
		SMaterial o;
		o.Lighting = false;
		driver->setMaterial(o);

		SColor color = hit ? SColor(255, 0, 255, 0) : SColor(255, 255, 0, 0);
		driver->draw3DLine(start, end, color);
		if (hit) {
			float s = 0.05f;
			aabbox3df box(end - vector3df(s), end + vector3df(s));
			driver->draw3DBox(box, color);
		}
	}

	virtual const aabbox3df& getBoundingBox() const override { return bBox; }
	virtual u32 getMaterialCount() const override { return 0; }
};