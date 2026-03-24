#include "SoundManager.h"

#include "Application.h"
#include "RenderHelper.h"
#include "DebugConsole.h"

#include "irrlicht.h"
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib")

static Application* a = nullptr;
static RenderHelper* rh = nullptr;
static DebugConsole* d = nullptr;
static irr::scene::ISceneManager* i_smgr = nullptr;

SoundManager::SoundManager(Application* owner) {
	a = owner;
	rh = owner->GetRenderHelper();
	d = owner->GetDebugConsole();
}

bool SoundManager::Init() {
	i_sound = irrklang::createIrrKlangDevice();
	return i_sound;
}

bool SoundManager::Update(float dt) {
	if (!rh->getActiveCamera()) return false;

	irr::scene::ICameraSceneNode* c = rh->getActiveCamera();
	irr::scene::ISceneNode* l = rh->getActiveCameraLeft();
	irr::scene::ISceneNode* f = rh->getActiveCameraForward();
	if (!c || !l || !f) return false;

	irr::core::vector3df pos = c->getAbsolutePosition();
	irr::core::vector3df forward = f->getAbsolutePosition() - pos;
	forward.normalize();
	irr::core::vector3df up = c->getUpVector();
	irr::core::vector3df lVel = irr::core::vector3df();

	if (firstVel)
		firstVel = false;
	else {
		irr::core::vector3df last = irr::core::vector3df(lastCamPos.x, lastCamPos.y, lastCamPos.z);
		lVel = (pos - last) / dt;
	}
	lastCamPos = Vec3S{ pos.X, pos.Y, pos.Z };

	i_sound->setListenerPosition(pos, forward, lVel, up);
}