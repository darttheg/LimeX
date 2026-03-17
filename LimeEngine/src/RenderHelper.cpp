#include "RenderHelper.h"
#include <fstream>
#include <stdexcept>
#include "irrlicht.h"

#include "DebugConsole.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"
#include "Objects/Mesh.h"

#include "External/CGUIColoredText.h"
#include "External/CTextAnchorSceneNode.h"

static DebugConsole* d = nullptr;

void RenderHelper::Init(irr::IrrlichtDevice* device, DebugConsole* debug) {
	i_device = device;
	d = debug;

	i_smgr = i_device->getSceneManager();
	i_driver = i_device->getVideoDriver();
	i_gui = i_device->getGUIEnvironment();
	i_gpu = i_driver->getGPUProgrammingServices();
}

bool RenderHelper::guardRenderingCheck(std::string msg) {
	if (!i_device || !i_smgr) {
		if (msg.empty())
			d->Warn("Rendered objects cannot be created until the Lime window has been created!");
		else
			d->Warn(msg);
		return false;
	}
	return true;
}

irr::video::IImage* texToImg(irr::video::IVideoDriver* driver, irr::video::ITexture* tex) {
	if (!tex) return nullptr;

	irr::core::dimension2d<u32> texSize = tex->getSize();
	irr::video::ECOLOR_FORMAT format = tex->getColorFormat();
	void* pixelData = tex->lock();
	if (!pixelData) return nullptr;

	irr::video::IImage* image = driver->createImageFromData(format, texSize, pixelData);
	tex->unlock();

	return image;
}

irr::scene::IBillboardSceneNode* RenderHelper::createDebugNode(DEBUG3D_TYPE t) {
	if (!i_device) return nullptr;

	irr::video::SMaterial o;
	o.Lighting = false;
	o.MaterialType = irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
	o.FogEnable = false;
	o.ZWriteEnable = true;
	o.ZBuffer = irr::video::E_COMPARISON_FUNC::ECFN_ALWAYS;
	o.setFlag(irr::video::E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, false);

	irr::video::ITexture* tex = nullptr;

	switch (t) {
	case DEBUG3D_TYPE::EMPTY:
		tex = i_driver->getTexture("icons/debug-empty.png");
		break;
	default:
		break;
	}

	if (!tex) return nullptr;

	o.setTexture(0, tex);
	irr::scene::IBillboardSceneNode* out = i_smgr->addBillboardSceneNode();
	out->getMaterial(0) = o;
	out->setSize(irr::core::dimension2df(1.0f, 1.0f));

	return out;
}

irr::video::ITexture* RenderHelper::createTexture(int w, int h, const std::string& name) {
	if (!guardRenderingCheck()) return nullptr;

	std::string outName = name;
	if (name.empty()) {
		outName = "tex_" + std::to_string((int)(rand() / (double)RAND_MAX * 9999));
	}

	irr::video::ITexture* tex = i_driver->addTexture(irr::core::dimension2du(w, h), outName.c_str());
	if (!tex) {
		d->Warn("Could not create texture " + outName + " of size " + std::to_string(w) + "x" + std::to_string(h));
		return nullptr;
	}

	return tex;
}

irr::video::ITexture* RenderHelper::createTexture(const std::string& path) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::ITexture* tex = i_driver->getTexture(path.c_str());
	if (!tex) {
		d->Warn("Could not create texture from " + path);
		return nullptr;
	}

	return tex;
}

irr::video::ITexture* RenderHelper::cropTexture(irr::video::ITexture* tex, const Vec2& pos, const Vec2& dim) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* img = i_driver->createImage(tex, irr::core::vector2di(pos.getX(), pos.getY()), irr::core::dimension2du(dim.getX(), dim.getY()));
	std::string name = "cropped_";
	name += tex->getName().getInternalName().c_str();
	irr::video::ITexture* out = i_driver->addTexture(name.c_str(), img);

	return out;
}

irr::video::ITexture* RenderHelper::appendTexture(irr::video::ITexture* tex, irr::video::ITexture* toAppend, const Vec2& pos) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* srcBase = texToImg(i_driver, tex);
	irr::video::IImage* appendBase = texToImg(i_driver, toAppend);

	appendBase->copyTo(srcBase, irr::core::vector2di(pos.getX(), pos.getY()));
	std::string op = "appended_";
	op += tex->getName().getInternalName().c_str();

	irr::video::ITexture* out = i_driver->addTexture(op.c_str(), srcBase);
	srcBase->drop();
	appendBase->drop();

	return out;
}

Vec4 RenderHelper::getColor(irr::video::ITexture* tex, const Vec2& pos) {
	if (!guardRenderingCheck()) return Vec4();

	irr::video::IImage* img = texToImg(i_driver, tex);
	irr::video::SColor pCol = img->getPixel(pos.getX(), pos.getY());
	img->drop();

	return Vec4(pCol.getRed(), pCol.getGreen(), pCol.getBlue(), pCol.getAlpha());
}

irr::video::ITexture* RenderHelper::setColor(irr::video::ITexture* tex, const Vec2& pos, const Vec4& color) {
	if (!guardRenderingCheck()) return nullptr;

	irr::video::IImage* img = texToImg(i_driver, tex);
	img->getPixel(pos.getX(), pos.getY());

	irr::video::ITexture* out = i_driver->addTexture(tex->getName().getPath().c_str(), img);
	i_driver->removeTexture(tex);
	img->drop();

	return out;
}

void RenderHelper::keyColor(irr::video::ITexture* tex, const Vec4& color) {
	if (!guardRenderingCheck()) return;

	i_driver->makeColorKeyTexture(tex, irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}

void RenderHelper::setVertexColor(irr::scene::IAnimatedMeshSceneNode* m, const Vec4& color) {
	if (!guardRenderingCheck()) return;

	irr::scene::IMeshManipulator* meshManipulator = i_smgr->getMeshManipulator();
	meshManipulator->setVertexColors(m->getMesh(), irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
}

irr::scene::ISceneNode* RenderHelper::createEmptyNode() {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addEmptySceneNode();
}

irr::scene::ISceneNode* RenderHelper::createSkydomeNode(irr::video::ITexture* tex) {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addSkyDomeSceneNode(tex, 16, 8, 1.0, 2.0, 100.0);
}

irr::scene::IBillboardSceneNode* RenderHelper::createBillboardNode() {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addBillboardSceneNode();
}

irr::scene::CTextAnchorSceneNode* RenderHelper::createText3DNode(irr::gui::CGUIColoredText* src) {
	if (!guardRenderingCheck()) return nullptr;

	return new CTextAnchorSceneNode(i_smgr->getRootSceneNode(), i_smgr, i_gui, src);
}

irr::scene::ITriangleSelector* RenderHelper::createTriangleSelector(irr::scene::IAnimatedMeshSceneNode* m) {
	if (!m) return nullptr;

	return i_smgr->createOctreeTriangleSelector(m->getMesh(), m, 32);
}

static bool isValidFilePath(irr::IrrlichtDevice* dev, const std::string& path) {
	if (!dev) return false;
	irr::io::IFileSystem* fs = dev->getFileSystem();
	return fs->existFile(path.c_str());
}

irr::scene::IAnimatedMesh* RenderHelper::createMesh(const std::string& path) {
	if (!guardRenderingCheck()) return nullptr;

	if (!isValidFilePath(i_device, path)) {
		d->Warn("Could not load Mesh from path " + path + ": file does not exist.");
		return nullptr;
	}

	irr::scene::IAnimatedMesh* out = i_smgr->getMesh(path.c_str());
	if (!out) {
		d->Warn("Could not load Mesh from path " + path + ": Mesh could not be created from file.");
	}
	
	return out;
}

irr::scene::IAnimatedMeshSceneNode* RenderHelper::createEmptyMesh() {
	if (!guardRenderingCheck()) return nullptr;
	return i_smgr->addAnimatedMeshSceneNode(nullptr);
}

irr::scene::IAnimatedMeshSceneNode* RenderHelper::createAnimatedMesh(irr::scene::IAnimatedMesh* m) {
	if (!guardRenderingCheck()) return nullptr;
	if (!m) {
		irr::scene::IAnimatedMeshSceneNode* err = i_smgr->addAnimatedMeshSceneNode(i_smgr->getMesh("meshes/error.obj"));
		irr::video::SMaterial mat = irr::video::SMaterial();
		mat.Lighting = false;
		mat.AmbientColor = irr::video::SColor(255);
		mat.setTexture(0, i_driver->getTexture("error"));
		mat.setFlag(irr::video::E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, false);
		mat.UseMipMaps = false;
		err->getMaterial(0) = mat;
		return err;
	}

	irr::scene::IAnimatedMeshSceneNode* out = i_smgr->addAnimatedMeshSceneNode(m);
	m->drop();

	return out;
}

irr::scene::IMeshSceneNode* RenderHelper::createOctreeMesh(irr::scene::IAnimatedMeshSceneNode* m) {
	if (!guardRenderingCheck()) return nullptr;

	irr::scene::IMesh* sm = m->getMesh()->getMesh(0);
	return i_smgr->addOctreeSceneNode(sm);
}

Mesh RenderHelper::createCubeMesh(const Vec3& size) {
	if (!guardRenderingCheck()) return Mesh();
	const irr::scene::IGeometryCreator* g = i_smgr->getGeometryCreator();
	irr::scene::IMesh* out = g->createCubeMesh(irr::core::vector3df(size.getX(), size.getY(), size.getZ()));
	Mesh o = Mesh(out);
	return o;
}

Mesh RenderHelper::createSphereMesh(float r, int polyCount) {
	if (!guardRenderingCheck()) return Mesh();
	const irr::scene::IGeometryCreator* g = i_smgr->getGeometryCreator();
	irr::scene::IMesh* out = g->createSphereMesh(r, polyCount, polyCount);
	Mesh o = Mesh(out);
	return o;
}

Mesh RenderHelper::createCylinderMesh(float r, float l, int polyCount, bool closed) {
	if (!guardRenderingCheck()) return Mesh();
	const irr::scene::IGeometryCreator* g = i_smgr->getGeometryCreator();
	irr::scene::IMesh* out = g->createCylinderMesh(r, l, polyCount, irr::video::SColor(255), closed);
	Mesh o = Mesh(out);
	return o;
}

Mesh RenderHelper::createPlaneMesh(const Vec2& tileSize, const Vec2& tileCount, const Vec2& texRepeat) {
	if (!guardRenderingCheck()) return Mesh();
	const irr::scene::IGeometryCreator* g = i_smgr->getGeometryCreator();
	irr::scene::IMesh* out = g->createPlaneMesh(irr::core::dimension2df(tileSize.getX(), tileSize.getY()), irr::core::dimension2du(tileCount.getX(), tileCount.getY()), 0, irr::core::dimension2df(texRepeat.getX(), texRepeat.getY()));
	Mesh o = Mesh(out);
	return o;
}

#include <sol/table.hpp>
sol::table RenderHelper::fireRaycast(const Vec3& start, const Vec3& end, float life) {
	scene::ISceneCollisionManager* collisionManager = i_smgr->getSceneCollisionManager();
	core::line3d<f32> ray(core::vector3df(start.getX(), start.getY(), start.getZ()), core::vector3df(end.getX(), end.getY(), end.getZ()));

	core::vector3df hitPosition;
	core::triangle3df hitTriangle;
	scene::ISceneNode* pickedNode = collisionManager->getSceneNodeAndCollisionPointFromRay(ray, hitPosition, hitTriangle, false);

	// Well... how do we create Lua table here? We shouldn't...

	return sol::table();
}

irr::scene::ICameraSceneNode* RenderHelper::createCameraNode() {
	if (!guardRenderingCheck()) return nullptr;

	return i_smgr->addCameraSceneNode();
}

void RenderHelper::updateCameraMatrix(irr::scene::ICameraSceneNode* c) {
	if (!c) return;

	if (c->isTrulyOrthogonal) {
		irr::core::matrix4 orthoMat;
		float z = c->getFOV() * 180.0 / irr::core::PI / 5.0;
		int width = i_driver->getScreenSize().Width;
		int height = i_driver->getScreenSize().Height;
		orthoMat.buildProjectionMatrixOrthoLH(width / z, height / z, c->getNearValue(), c->getFarValue());
		c->setProjectionMatrix(orthoMat, true);
	}
	else {
		irr::core::matrix4 perspectiveMat;
		float aspectRatio = c->getAspectRatio();
		perspectiveMat.buildProjectionMatrixPerspectiveFovLH(c->getFOV(), aspectRatio, c->getNearValue(), c->getFarValue());

		c->setProjectionMatrix(perspectiveMat, false);
	}
}

void RenderHelper::setActiveCamera(irr::scene::ICameraSceneNode* c) {
	if (!guardRenderingCheck()) return;
	i_smgr->setActiveCamera(c);
}

irr::gui::CGUIColoredText* RenderHelper::createColoredText2D() {
	if (!guardRenderingCheck()) return nullptr;

	irr::core::rect<irr::s32> r(0, 0, 128, 64);
	irr::gui::CGUIColoredText* out = new irr::gui::CGUIColoredText(i_gui, i_gui->getRootGUIElement(), -1, r);

	return out;
}

irr::gui::IGUIStaticText* RenderHelper::createStaticText() {
	if (!guardRenderingCheck()) return nullptr;

	return i_gui->addStaticText(L"", irr::core::recti(0, 0, 64, 64));
}

irr::gui::IGUIImage* RenderHelper::createGUIImage() {
	if (!guardRenderingCheck()) return nullptr;

	irr::gui::IGUIImage* img = i_gui->addImage(irr::core::recti(0, 0, 64, 64));
	img->setImage(i_driver->getTexture("blank"));

	return img;
}

irr::gui::IGUIButton* RenderHelper::createButton() {
	if (!guardRenderingCheck()) return nullptr;
	irr::gui::IGUIButton* out = i_gui->addButton(irr::core::recti());
	out->setDrawBorder(false);

	return out;
}

irr::gui::IGUIElement* RenderHelper::getGUIRoot() {
	return i_gui ? i_gui->getRootGUIElement() : nullptr;
}