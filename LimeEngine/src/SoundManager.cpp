#include "SoundManager.h"
#include "External/SoundMini.h"

#ifdef __ANDROID__
#include "LimeAndroid.h"
static ma_engine* pauseEngine = nullptr;
#endif

#include "Application.h"
#include "RenderHelper.h"
#include "DebugConsole.h"
#include "Window.h"

#include "irrlicht.h"
#include <algorithm>
#include <filesystem>

static Application* a = nullptr;
static RenderHelper* rh = nullptr;
static DebugConsole* d = nullptr;

// Audio files above this size will stream if auto type
static const std::uintmax_t autoStreamBytes = 4 * 1024 * 1024;

SoundManager::SoundManager(Application* owner) {
	a = owner;
	rh = owner->GetRenderHelper();
	d = owner->GetDebugConsole();
}

bool SoundManager::guardSoundCheck(std::string msg) {
	if (!ready) {
		std::string out = "Interaction with sound components is forbidden until the Lime window has been created.";
		if (!msg.empty()) out = msg;
		d->PostError(out, true, true);
		return false;
	}
	return true;
}

bool SoundManager::Init() {
	engine = std::make_unique<ma_engine>();
	if (ma_engine_init(nullptr, engine.get()) != MA_SUCCESS) {
		ma_engine_config cfg = ma_engine_config_init();
		cfg.noDevice = MA_TRUE;
		cfg.channels = 2;
		cfg.sampleRate = 48000;
		if (ma_engine_init(&cfg, engine.get()) != MA_SUCCESS) {
			engine.reset();
			return false;
		}
		d->Warn("No audio device found, sound is disabled.");
	}
	ready = true;

#ifdef __ANDROID__
	pauseEngine = engine.get();
	LimeAndroid::setPauseListener([](bool paused) {
		if (paused) ma_engine_stop(pauseEngine); else ma_engine_start(pauseEngine);
		});
#endif

	return true;
}

void SoundManager::Shutdown() {
	if (!ready) return;
	stopAllSounds();
	ma_engine_stop(engine.get());
}

bool SoundManager::Update(float dt) {
	if (!ready) return false;
	ma_engine* e = engine.get();

	soundMinis.erase(std::remove_if(soundMinis.begin(), soundMinis.end(),
		[](const std::shared_ptr<SoundMini>& v) { return v->isFinished(); }),
		soundMinis.end());

	if (muteUnfocus && !a->GetWindow()->isFocused())
		ma_engine_set_volume(e, 0.0f);
	else
		ma_engine_set_volume(e, mainVol);

	if (!rh->getActiveCamera()) return false;

	irr::scene::ICameraSceneNode* camNode = rh->getActiveCamera();
	irr::scene::ISceneNode* leftNode = rh->getActiveCameraLeft();
	irr::scene::ISceneNode* forwardNode = rh->getActiveCameraForward();

	if (!camNode || !leftNode || !forwardNode) return false;

	irr::core::vector3df pos = camNode->getAbsolutePosition();
	irr::core::vector3df forward = forwardNode->getAbsolutePosition() - pos;
	forward.normalize();
	irr::core::vector3df up = camNode->getUpVector();
	irr::core::vector3df lVel = irr::core::vector3df();

	if (firstVel || last != camNode)
		firstVel = false;
	else {
		irr::core::vector3df last = irr::core::vector3df(lastCamPos.x, lastCamPos.y, lastCamPos.z);
		lVel = (pos - last) / dt;
	}

	lastCamPos = Vec3S{ pos.X, pos.Y, pos.Z };
	last = camNode;

	irr::core::vector3df vel = lVel * velFactor * distanceFactor;
	ma_engine_listener_set_position(e, 0, pos.X, pos.Y, -pos.Z);
	ma_engine_listener_set_direction(e, 0, forward.X, forward.Y, -forward.Z);
	ma_engine_listener_set_velocity(e, 0, vel.X, vel.Y, -vel.Z);
	ma_engine_listener_set_world_up(e, 0, up.X, up.Y, -up.Z);

	for (auto it = soundNodePairs.begin(); it != soundNodePairs.end();) {
		auto v = it->sound.lock();
		if (!v || !it->parent || v->isFinished()) {
			it = soundNodePairs.erase(it);
		} else {
			it->parent->updateAbsolutePosition();
			auto p = it->parent->getAbsolutePosition();
			ma_sound_set_position(&v->sound, p.X, p.Y, -p.Z);
			++it;
		}
	}

	return true;
}

int SoundManager::getMainVolume() {
	return ready ? (int)(ma_engine_get_volume(engine.get()) * 100) : 0;
}

void SoundManager::setMainVolume(int v) {
	if (!guardSoundCheck()) return;
	float out = (float)(v / 100.0);
	ma_engine_set_volume(engine.get(), out);
	mainVol = out;
}

void SoundManager::setAllSoundsPaused(bool v) {
	if (!guardSoundCheck()) return;
	for (auto& mini : soundMinis)
		mini->setPaused(v);
}

void SoundManager::stopAllSounds() {
	if (!guardSoundCheck()) return;
	for (auto& mini : soundMinis)
		mini->stop();
	soundMinis.clear();
	soundNodePairs.clear();
}

int SoundManager::getLoadedSoundsCount() {
	return (int)sources.size();
}

void SoundManager::setDefaultMin(float min) {
	if (!guardSoundCheck()) return;
	defaultMin = min;
}

void SoundManager::setDefaultMax(float max) {
	if (!guardSoundCheck()) return;
	defaultMax = max;
}

void SoundManager::setMuteUnfocus(bool v) {
	muteUnfocus = v;
}

void SoundManager::setDopplerEffectParameters(float doppler, float dist) {
	if (!guardSoundCheck()) return;
	dopplerFactor = doppler;
	distanceFactor = dist;
	for (auto& mini : soundMinis)
		ma_sound_set_doppler_factor(&mini->sound, doppler);
}

SoundData* SoundManager::createSoundSource(const std::string& path, int type) {
	if (!guardSoundCheck()) return nullptr;
	
	auto it = sources.find(path);
	if (it != sources.end()) return it->second.get();

	std::error_code ec;
	auto size = std::filesystem::file_size(path, ec);
	if (ec) return nullptr;

	auto src = std::make_unique<SoundData>();
	src->path = path;
	src->stream = type == 1 || (type == 0 && size > autoStreamBytes);

	if (src->stream) {
		ma_decoder dec;
		if (ma_decoder_init_file(path.c_str(), nullptr, &dec) != MA_SUCCESS) return nullptr;
		ma_decoder_uninit(&dec);
	} else {
		ma_resource_manager* rm = ma_engine_get_resource_manager(engine.get());
		if (ma_resource_manager_register_file(rm, path.c_str(), MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE) != MA_SUCCESS)
			return nullptr;
	}

	SoundData* out = src.get();
	sources.emplace(path, std::move(src));
	return out;
}

void SoundManager::unloadSound(SoundData* src) {
	if (!guardSoundCheck() || !src) return;
	if (!src->stream)
		ma_resource_manager_unregister_file(ma_engine_get_resource_manager(engine.get()), src->path.c_str());
	sources.erase(src->path);
}

std::shared_ptr<SoundMini> SoundManager::play(SoundData* src, bool td, bool loops) {
	if (!guardSoundCheck() || !src) return nullptr;

	auto v = std::make_shared<SoundMini>();
	v->engine = engine.get();

	ma_uint32 flags = src->stream ? MA_SOUND_FLAG_STREAM : MA_SOUND_FLAG_DECODE;
	if (!td) flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;
	if (ma_sound_init_from_file(engine.get(), src->path.c_str(), flags, nullptr, nullptr, &v->sound) != MA_SUCCESS) {
		d->Warn("Could not play sound from path " + src->path);
		return nullptr;
	}
	v->ready = true;

	ma_sound_set_looping(&v->sound, loops);
	if (td) {
		ma_sound_set_attenuation_model(&v->sound, ma_attenuation_model_inverse);
		ma_sound_set_min_distance(&v->sound, defaultMin);
		ma_sound_set_max_distance(&v->sound, defaultMax);
		ma_sound_set_doppler_factor(&v->sound, dopplerFactor);
	}

	soundMinis.push_back(v);
	return v;
}

bool SoundManager::attachSoundToNode(const std::shared_ptr<SoundMini>& sound, irr::scene::ISceneNode* parent) {
	if (!guardSoundCheck() || !sound || !parent) return false;

	for (auto& pair : soundNodePairs) {
		if (pair.sound.lock() == sound) {
			pair.parent = parent;
			return true;
		}
	}

	soundNodePairs.push_back(SoundSourceOnNode{ sound, parent });
	return true;
}

void SoundManager::detachSoundFromNode(const SoundMini* sound) {
	if (!sound) return;

	for (auto it = soundNodePairs.begin(); it != soundNodePairs.end(); ++it) {
		if (it->sound.lock().get() == sound) {
			soundNodePairs.erase(it);
			return;
		}
	}
}

void SoundManager::warnGarbageCollection(const std::string& path) {
	a->warnGarbageCollection(path);
}

bool SoundManager::preloadSound(const std::string& path) {
	return createSoundSource(path, 0) != nullptr;
}
