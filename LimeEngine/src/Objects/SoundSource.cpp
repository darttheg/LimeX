#include "Objects/SoundSource.h"

#include "RenderHelper.h"
#include "SoundManager.h"
#include "irrKlang.h"
#include <sol/sol.hpp>
#include "Objects/Vec3.h"
#include "Interfaces/Object3D.h"
#include "Objects/DebugAxisPlaneNode.h"

static SoundManager* s = nullptr;
static RenderHelper* rh = nullptr;
static lua_State* l = nullptr;

SoundSource::SoundSource() {
}

SoundSource::SoundSource(const std::string& path, int type) {
	loadFromFile(path, type);
	if (!src) return;
}

bool SoundSource::play(bool td) {
	if (!src) return false;
	if (cur) {
		cur->stop();
		cur->drop();
	}

	cur = s->play(src, td, loops, doSFX);
	if (cur && td)
		cur->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
	if (cur) {
		cur->setMinDistance(minDist);
		cur->setMaxDistance(maxDist);
		cur->setVolume(vol);
	}
	is3D = td;

	return cur;
}

void SoundSource::stop() {
	if (!src || !cur) return;
	cur->stop();
	cur->drop();
	parent = nullptr;
	is3D = false;
}

bool SoundSource::isPlaying() {
	if (!src || !cur) return false;
	return !cur->isFinished();
}

bool SoundSource::getPaused() {
	if (!src || !cur) return false;
	return cur->getIsPaused();
}

void SoundSource::setPaused(bool v) {
	if (!src || !cur) return;
	cur->setIsPaused(v);
}

bool SoundSource::getLooping() {
	return cur ? cur->isLooped() : loops;
}

void SoundSource::setLooping(bool v) {
	loops = v;
	if (cur) cur->setIsLooped(v);
}

float SoundSource::getMinDist() {
	return cur ? cur->getMinDistance() : 0.0f;
}

void SoundSource::setMinDist(float f) {
	if (!src || !cur) return;
	cur->setMinDistance(f);
	minDist = f;
}

float SoundSource::getMaxDist() {
	return cur ? cur->getMaxDistance() : 0.0f;
}

void SoundSource::setMaxDist(float f) {
	if (!src || !cur) return;
	cur->setMaxDistance(f);
	maxDist = f;
}

void SoundSource::setVolume(float f) {
	if (!src) return;
	vol = f;
	if (cur) cur->setVolume(f);
}

float SoundSource::getVolume() {
	return cur ? cur->getVolume() : vol;
}

void SoundSource::setPitch(float f) {
	if (!src || !cur) return;
	cur->setPlaybackSpeed(f);
}

float SoundSource::getPitch() {
	return cur ? cur->getPlaybackSpeed() : 0.0f;
}

void SoundSource::setPan(float f) {
	if (!src || !cur) return;
	cur->setPan(f);
}

float SoundSource::getPan() {
	return cur ? cur->getPan() : 0.0f;
}

int SoundSource::getPlayPosition() {
	return cur ? cur->getPlayPosition() : 0;
}

void SoundSource::setPlayPosition(int ms) {
	if (!src || !cur) return;
	cur->setPlayPosition(ms);
}

int SoundSource::getPlayLength() {
	return cur ? cur->getPlayLength() : 0;
}

void SoundSource::setVelocity(const Vec3& vel) {
	if (!src || !cur) return;
	cur->setVelocity(irrklang::vec3df(vel.getX(), vel.getY(), vel.getZ()));
}

Vec3 SoundSource::getVelocity() {
	return cur ? Vec3(cur->getVelocity().X, cur->getVelocity().Y, cur->getVelocity().Z) : Vec3();
}

void SoundSource::setPosition(const Vec3& p) {
	pos.x = p.getX();
	pos.y = p.getY();
	pos.z = p.getZ();

	if (!src || !cur) return;
	cur->setPosition(irrklang::vec3df(p.getX(), p.getY(), p.getZ()));

	if (dVisual && dAxis) {
		irr::core::vector3df out;
		if (parent)
			out = parent->getAbsolutePosition();
		else
			out = irr::core::vector3df(cur->getPosition().X, cur->getPosition().Y, cur->getPosition().Z);
		dVisual->setPosition(out);
		dAxis->setPosition(out);
	}
}

Vec3 SoundSource::getPosition() {
	return cur ? Vec3(pos.x, pos.y, pos.z) : Vec3();
}

bool SoundSource::getDoSFX() {
	return doSFX;
}

void SoundSource::setDoSFX(bool v) {
	doSFX = v;
}

bool SoundSource::getDebug() {
	return cur && is3D && dVisual;
}

void SoundSource::setDebug(bool v) {
	if (v) {
		if (!is3D || !cur) return;
		if (dVisual) { dVisual->drop(); dVisual->remove(); }
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::SOUND);
		dAxis = new DebugAxisPlaneNode(dVisual->getSceneManager()->getRootSceneNode(), dVisual->getSceneManager());
		setPosition(getPosition());
	} else {
		if (dVisual) {
			dVisual->drop();
			dVisual->remove();
			dVisual = nullptr;
		}
		if (dAxis) {
			dAxis->drop();
			dAxis->remove();
			dAxis = nullptr;
		}
	}
}

bool SoundSource::attachTo(sol::optional<Object3D*> p) {
	if (!cur || !is3D) return false;

	if (!p || *p == nullptr) {
		parent = nullptr; // SoundManager update will resolve
		return true;
	}

	Object3D* pa = *p;
	if (!pa->getNode()) return false;
	parent = pa->getNode();
	return s->attachSoundToNode(cur, parent);
}

bool SoundSource::isAttached() {
	return src && cur && parent;
}

std::string SoundSource::getPath() {
	return src ? src->getName() : "";
}

void SoundSource::collected() {
	if (src) s->warnGarbageCollection(src->getName());
}

sol::object SoundSource::destroy() {
	cur->stop();
	cur = nullptr;
	src = nullptr;
	return sol::make_object(l, sol::nil);
}

sol::object SoundSource::purge() {
	if (cur) cur->stop();
	s->unloadSound(src);
	cur = nullptr;
	src = nullptr;
	return sol::make_object(l, sol::nil);
}

bool SoundSource::loadFromFile(const std::string& path, int type) {
	stop();
	src = s->createSoundSource(path, type);
	return src;
}

void SoundSource::clearEffects() {
	if (!cur) return;
	cur->getSoundEffectControl()->disableAllEffects();
}

bool SoundSource::addDistortionEffect(float gain, float edge) {
	if (!cur || !doSFX) return false;
	if (!cur->getSoundEffectControl()) return false;
	cur->getSoundEffectControl()->enableDistortionSoundEffect(gain, edge);
	return true;
}

bool SoundSource::addEchoEffect(float wetDry, float feedback, float delay) {
	if (!cur || !doSFX) return false;
	if (!cur->getSoundEffectControl()) return false;
	cur->getSoundEffectControl()->enableEchoSoundEffect(wetDry, feedback, delay, delay);
	return true;
}

bool SoundSource::addReverbEffect(float inputGain, float mix, float time, float freqRatio) {
	if(!cur || !doSFX) return false;
	if (!cur->getSoundEffectControl()) return false;
	cur->getSoundEffectControl()->enableWavesReverbSoundEffect(inputGain, mix, time, freqRatio);
	return true;
}

bool SoundSource::addCompressionEffect(float threshold, float ratio) {
	if (!cur || !doSFX) return false;
	if (!cur->getSoundEffectControl()) return false;
	cur->getSoundEffectControl()->enableCompressorSoundEffect(0.0f, 10.0f, 200.0f, threshold, ratio, 4.0f);
	return true;
}

bool SoundSource::addParamEqEffect(float fCenter, float fBandwidth, float fGain) {
	if (!cur || !doSFX) return false;
	if (!cur->getSoundEffectControl()) return false;
	cur->getSoundEffectControl()->enableParamEqSoundEffect(fCenter, fBandwidth, fGain);
	return true;
}

void Object::SoundSourceBind::bind(lua_State* ls, SoundManager* sou, RenderHelper* renh) {
	l = ls;
	s = sou;
	rh = renh;

	// Object SoundSource, A source of sound, whether that be for sound effects or music.

	// Constructor
	// Constructor string path, Lime.Enum.SoundType? type

	sol::state_view view(ls);
	sol::usertype<SoundSource> obj = view.new_usertype<SoundSource>(
		"SoundSource",
		sol::constructors<SoundSource(), SoundSource(const std::string&), SoundSource(const std::string&, int)>(),

		sol::meta_function::type, [](const SoundSource&) { return "SoundSource"; },
		sol::meta_function::garbage_collect, [](SoundSource& ss) { ss.collected(); },

		// Field boolean paused, Whether or not this `SoundSource` is paused.
		"paused", sol::property(&SoundSource::getPaused, &SoundSource::setPaused),

		// Field boolean looping, Whether or not this `SoundSource` loops on playback. 
		"looping", sol::property(&SoundSource::getLooping, &SoundSource::setLooping),

		// Field number volume, The volume of this `SoundSource`.
		"volume", sol::property(&SoundSource::getVolume, &SoundSource::setVolume),

		// Field number speed, The playback speed of this `SoundSource`.
		"speed", sol::property(&SoundSource::getPitch, &SoundSource::setPitch),

		// Field number pan, The pan of this `SoundSource`, where -1.0 is left and 1.0 is right. 
		"pan", sol::property(&SoundSource::getPan, &SoundSource::setPan),

		// Field number minimumDistance, Sets the minimum listening distance for this `SoundSource`. Only applicable if this object is played in 3D.
		"minimumDistance", sol::property(&SoundSource::getMinDist, &SoundSource::setMinDist),

		// Field number maximumDistance, Sets the maximum listening distance for this `SoundSource`. Only applicable if this object is played in 3D.
		"maximumDistance", sol::property(&SoundSource::getMaxDist, &SoundSource::setMaxDist),

		// Field number playbackPosition, The current playback position of this `SoundSource`.
		"playbackPosition", sol::property(&SoundSource::getPlayPosition, &SoundSource::setPlayPosition),

		// Field Vec3 velocity, The velocity of this `SoundSource`. Only applicable if this object is played in 3D.
		"velocity", sol::property(
			[](SoundSource& c) { return Vec3{ [&] { return c.getVelocity(); }, [&](auto v) { c.setVelocity(v); } }; },
			[](SoundSource& c, const Vec3& v) { c.setVelocity(v); }
		),

		// Field Vec3 position, The position of this `SoundSource` in the scene. Only applicable if this `SoundSource` is played in 3D.
		"position", sol::property(
			[](SoundSource& c) { return Vec3{ [&] { return c.getPosition(); }, [&](auto v) { c.setPosition(v); } }; },
			[](SoundSource& c, const Vec3& v) { c.setPosition(v); }
		),

		// Field boolean effects, Whether or not sound effects are enabled on playback. This flag must first be enabled to apply effects, as it is false by default. Sound effects are more resource-intensive.
		"effects", sol::property(&SoundSource::getDoSFX, &SoundSource::setDoSFX),

		// Field boolean debug, Show debug information about this object in the scene.
		"debug", sol::property(&SoundSource::getDebug, &SoundSource::setDebug)
	);

	obj[sol::meta_function::to_string] = [](const SoundSource& v) {
		return "SoundSource";
		};

	// Play this `SoundSource`.
	// Params boolean? is3D
	// Returns boolean
	obj.set_function("play", &SoundSource::play);

	// Stop this `SoundSource`.
	// Returns void
	obj.set_function("stop", &SoundSource::stop);

	// Returns true if this `SoundSource` is playing.
	// Returns boolean
	obj.set_function("isPlaying", &SoundSource::isPlaying);

	// Returns the playback length of this `SoundSource`.
	// Returns number
	obj.set_function("getLength", &SoundSource::getPlayLength);

	// Returns the file path of the sound loaded into this `SoundSource`.
	// Returns string
	obj.set_function("getPath", &SoundSource::getPath);

	// Parents this `SoundSource` to a 3D object. (NOTE: This `SoundSource` must be playing in 3D)
	// Params any parent
	// Returns boolean
	obj.set_function("parentTo", &SoundSource::attachTo);

	// Returns true if this `SoundSource` is parented to a 3D object.
	// Returns boolean
	obj.set_function("hasParent", &SoundSource::isAttached);

	// Loads a new sound into this `SoundSource`. (WARNING: Unused sounds should be purged to free up unused memory)
	// Params string path, Lime.Enum.SoundType? type
	// Returns boolean
	obj.set_function("load", &SoundSource::loadFromFile);

	// Destroys this `SoundSource`, which stops itself from playing in the scene as well as detaching from a parent 3D object. To free this sound from memory, see `SoundSource:purge`.
	// Returns nil
	obj.set_function("destroy", &SoundSource::destroy);

	// Purges this `SoundSource`, effectively removing it from memory. If other `SoundSource` objects use this sound, there may be issues.
	// Returns nil
	obj.set_function("purge", &SoundSource::purge);

	// Clears all effects applied to this `SoundSource`. Stopping or destroying this `SoundSource` will clear its effects.
	// Returns void
	obj.set_function("clearEffects", &SoundSource::clearEffects);

	// Enables distortion on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect messes with the sound's frequency and other attributes to produce an odd result.
	// Params
	// Params number gain, number edge
	// Returns bool
	obj.set_function("addDistortionEffect", &SoundSource::addDistortionEffect);

	// Enables echoing on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect repeats the sound with decay over time.
	// Params
	// Params number wetDry, number feedback, number delayMs
	// Returns bool
	obj.set_function("addEchoEffect", &SoundSource::addEchoEffect);

	// Enables reverb on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect mixes the sound to bounce off surfaces in a room or a cave.
	// Params
	// Params number inputGain, number mix, number timeMs, number freqRatio
	// Returns bool
	obj.set_function("addReverbEffect", &SoundSource::addReverbEffect);

	// Enables compression on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect reduces the dynamic range of the sound's waveform.
	// Params
	// Params number threshold, number ratio
	// Returns bool
	obj.set_function("addCompressionEffect", &SoundSource::addCompressionEffect);

	// Enables parametric equilization on this `SoundSource`. Only applicable if this `SoundSource` is playing. This effect amplifies or attenuates signals at a given frequency.
	// Params
	// Params number threshold, number ratio
	// Returns bool
	obj.set_function("addParamEqEffect", &SoundSource::addParamEqEffect);

	// End Object
}
