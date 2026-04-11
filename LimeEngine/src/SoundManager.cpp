#include "SoundManager.h"

#include "Application.h"
#include "RenderHelper.h"
#include "DebugConsole.h"
#include "Window.h"

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

bool SoundManager::guardSoundCheck(std::string msg) {
	if (!i_sound) {
		if (msg.empty())
			d->Warn("Sound system cannot be modified until the Lime window has been created!");
		else
			d->Warn(msg);
		return false;
	}
	return true;
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

	if (firstVel || last != c) // Do not compute velocity if cameras are different by end of Lua cycle
		firstVel = false;
	else {
		irr::core::vector3df last = irr::core::vector3df(lastCamPos.x, lastCamPos.y, lastCamPos.z);
		lVel = (pos - last) / dt;
	}
	lastCamPos = Vec3S{ pos.X, pos.Y, pos.Z };
	last = c;

	i_sound->setListenerPosition(pos, forward, lVel * velFactor, up);

	// Sound Node Pairs
	for (auto it = soundNodePairs.begin(); it != soundNodePairs.end();) {
		if (!it->sound || !it->parent) { // || s.sound->isFinished() ?
			it = soundNodePairs.erase(it);
		} else {
			auto pos = it->parent->getAbsolutePosition();
			it->sound->setPosition(pos);
			++it;
		}
	}

	if (muteUnfocus && !a->GetWindow()->isFocused()) {
		i_sound->setSoundVolume(0.0f);
	} else {
		i_sound->setSoundVolume(mainVol);
	}

	return true;
}

int SoundManager::getMainVolume() {
	return i_sound ? (int)(i_sound->getSoundVolume() * 100) : 0;
}

void SoundManager::setMainVolume(int v) {
	if (!guardSoundCheck()) return;
	float out = (float)(v / 100.0);
	i_sound->setSoundVolume(out);
	mainVol = out;
}

void SoundManager::setAllSoundsPaused(bool v) {
	i_sound->setAllSoundsPaused(v);
}

void SoundManager::stopAllSounds() {
	i_sound->stopAllSounds();
	soundNodePairs.clear();
}

int SoundManager::getLoadedSoundsCount() {
	return i_sound->getSoundSourceCount();
}

void SoundManager::setDefaultMin(float min) {
	i_sound->setDefault3DSoundMinDistance(min);
}

void SoundManager::setDefaultMax(float max) {
	i_sound->setDefault3DSoundMaxDistance(max);
}

void SoundManager::setMuteUnfocus(bool v) {
	muteUnfocus = v;
}

void SoundManager::setDopplerEffectParameters(float dopplerFactor, float distanceFactor) {
	if (!guardSoundCheck()) return;
	i_sound->setDopplerEffectParameters(dopplerFactor, distanceFactor);
}

irrklang::ISoundSource* SoundManager::createSoundSource(const std::string& path, int type) {
	if (!guardSoundCheck()) return nullptr;
	auto* out = i_sound->addSoundSourceFromFile(path.c_str(), (irrklang::E_STREAM_MODE)type, true);
	if (!out) { // Found, but let's return the source.
		out = i_sound->getSoundSource(path.c_str());
	}
	return out;
}

void SoundManager::unloadSound(irrklang::ISoundSource* src) {
	if (!guardSoundCheck() || !src) return;
	i_sound->removeSoundSource(src);
	src = nullptr;
}

irrklang::ISound* SoundManager::play(irrklang::ISoundSource* src, bool td, bool loops, bool sfx) {
	if (!guardSoundCheck() || !src) return nullptr;

	irrklang::ISound* out = nullptr;
	if (td)
		out = i_sound->play3D(src, irrklang::vec3df(), loops, false, true, sfx);
	else
		out = i_sound->play2D(src, loops, false, true, sfx);

	std::string s = src->getName();
	if (!out) d->Warn("Could not play sound from path " + s);
	return out;
}

bool SoundManager::attachSoundToNode(irrklang::ISound* sound, irr::scene::ISceneNode* parent) {
	if (!guardSoundCheck() || !sound || !parent) return false;

	// Make sure sound is not an entry already. it->parent will point to the param parent already if it is already an entry.
	for (auto it = soundNodePairs.begin(); it != soundNodePairs.end();) {
		if (it->sound == sound) return true;
	}

	soundNodePairs.push_back(SoundSourceOnNode{ sound, parent });
	return true;
}

void SoundManager::warnGarbageCollection(const std::string& path) {
	a->warnGarbageCollection(path);
}