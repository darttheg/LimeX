#pragma once
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class DebugAxisPlaneNode : public ISceneNode {
private:
	aabbox3df bBox;
	float dirLen = 0.0f;
	SColor lenColor = SColor(255, 255, 255, 0);
public:
	DebugAxisPlaneNode(ISceneNode* parent, ISceneManager* smgr, s32 id = -1) : ISceneNode(parent, smgr, id) {}

	virtual void OnRegisterSceneNode() override {
		if (IsVisible) SceneManager->registerNodeForRendering(this, ESNRP_TRANSPARENT);
		ISceneNode::OnRegisterSceneNode();
	}

	virtual void OnAnimate(u32 timeMs) override {
		ISceneNode::OnAnimate(timeMs);
	}

	void setPointerLength(float f) {
		dirLen = f;
	}

	void setPointerColor(SColorf c) {
		lenColor = SColor(c.r * 255, c.g * 255, c.b * 255, c.a * 255);
	}

	virtual void render() override {
		IVideoDriver* driver = SceneManager->getVideoDriver();
		SMaterial o;
		o.Lighting = false;
		o.FogEnable = false;
		/*
		o.ZWriteEnable = true;
		o.ZBuffer = irr::video::E_COMPARISON_FUNC::ECFN_ALWAYS;
		*/
		driver->setMaterial(o);

		SColor r = SColor(255, 255, 0, 0);
		SColor g = SColor(255, 0, 255, 0);
		SColor b = SColor(255, 0, 0, 255);

		f32* m = AbsoluteTransformation.pointer();
		vector3df origin(m[12], m[13], m[14]);
		vector3df axisX(1, 0, 0);
		vector3df axisY(0, 1, 0);
		vector3df axisZ(0, 0, 1);
		vector3df forward(m[8], m[9], m[10]);

		float l = 1.0f; // Tie to scale?
		driver->setTransform(ETS_WORLD, core::IdentityMatrix);
		driver->draw3DLine(origin, origin + axisX * l, r);
		driver->draw3DLine(origin, origin + axisY * l, b);
		driver->draw3DLine(origin, origin + axisZ * l, g);
		driver->draw3DLine(origin, origin + forward * dirLen, lenColor);
		driver->setTransform(ETS_WORLD, AbsoluteTransformation);
	}

	virtual const aabbox3df& getBoundingBox() const override { return bBox; }
	virtual u32 getMaterialCount() const override { return 0; }
};