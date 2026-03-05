#include "RenderHelper.h"
#include <fstream>
#include <stdexcept>
#include "irrlicht.h"

#include "DebugConsole.h"

#include "Objects/Vec2.h"
#include "Objects/Vec3.h"
#include "Objects/Vec4.h"
#include "Objects/Texture.h"

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

bool RenderHelper::guardRenderingCheck() {
	if (!i_device) {
		d->Warn("Rendered objects cannot be created until the Lime window has been created!");
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
	out->setSize(irr::core::dimension2df(1.5, 1.5));

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