#pragma once
#include <irrlicht.h>

using namespace irr;

class ShadowVolumeSceneNode : public scene::ISceneNode {
public:
	inline static bool DrawingThisFrame = false;

	ShadowVolumeSceneNode(scene::IMesh* mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
		: ISceneNode(parent, mgr, id), source(mesh) {
		if (source) { source->grab(); box = source->getBoundingBox(); }
	}

	virtual void OnRegisterSceneNode() override {
		if (IsVisible) SceneManager->registerNodeForRendering(this, scene::ESNRP_SHADOW);
		ISceneNode::OnRegisterSceneNode();
	}

	bool loadMesh(scene::IMesh* mesh) {
		if (!mesh) return false;
		if (source) source->drop();
		source = mesh;
		source->grab();
		box = source->getBoundingBox();
		return true;
	}

	void doStencil() {
		video::IVideoDriver* driver = SceneManager->getVideoDriver();
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

		bool doDraw = true;
		if (irr::scene::ICameraSceneNode* c = SceneManager->getActiveCamera()) {
			core::aabbox3d<f32> wBox = box;
			AbsoluteTransformation.transformBoxEx(wBox);
			const scene::SViewFrustum* frust = c->getViewFrustum();
			if (!frust->getBoundingBox().intersectsWithBox(wBox)) doDraw = false;
		}

		if (doDraw) {
			DrawingThisFrame = true;
			updateTriangles();
			driver->drawStencilShadowVolume(triangles, true, scene::EDS_OFF);
		} else {
			core::array<core::vector3df> empty;
			driver->drawStencilShadowVolume(empty, true, scene::EDS_OFF);
		}
	}

	virtual void render() override {
		doStencil();
	}

	virtual ~ShadowVolumeSceneNode() {
		if (source) source->drop();
	}

	virtual const core::aabbox3d<f32>& getBoundingBox() const override { return box; }
private:
	scene::IMesh* source;
	core::aabbox3d<f32> box;
	core::array<core::vector3df> triangles;

	void updateTriangles() {
		triangles.set_used(0);
		const core::matrix4& mat = AbsoluteTransformation;

		for (u32 b = 0; b < source->getMeshBufferCount(); ++b) {
			scene::IMeshBuffer* buf = source->getMeshBuffer(b);
			if (!buf) continue;

			u16* idx = buf->getIndices();
			u32 idxCount = buf->getIndexCount();

			for (u32 i = 0; i + 2 < idxCount; i += 3) {
				const core::vector3df a = buf->getPosition(idx[i + 0]);
				const core::vector3df b = buf->getPosition(idx[i + 1]);
				const core::vector3df c = buf->getPosition(idx[i + 2]);

				triangles.push_back(a);
				triangles.push_back(b);
				triangles.push_back(c);
			}
		}
	}
};