#pragma once
#include <irrlicht.h>

using namespace irr;

class ShadowVolumeSceneNode : public scene::ISceneNode {
public:
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
		updateWorldTriangles();
		driver->drawStencilShadowVolume(worldTriangles, true, scene::EDS_MESH_WIRE_OVERLAY);
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
	core::array<core::vector3df> worldTriangles;

	void updateWorldTriangles() {
		worldTriangles.set_used(0);
		const core::matrix4& mat = AbsoluteTransformation;

		for (u32 b = 0; b < source->getMeshBufferCount(); ++b) {
			scene::IMeshBuffer* buf = source->getMeshBuffer(b);
			u16* idx = buf->getIndices();
			u32 idxCount = buf->getIndexCount();

			for (u32 i = 0; i < idxCount; ++i) {
				core::vector3df pos = buf->getPosition(idx[i]);
				mat.transformVect(pos);
				worldTriangles.push_back(pos);
			}
		}
	}
};