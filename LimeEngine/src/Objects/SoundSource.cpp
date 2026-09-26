#include "Objects/SoundSource.h"

#include "RenderHelper.h"
#include "SoundManager.h"
#include <sol/sol.hpp>
#include "Objects/Vec3.h"
#include "Interfaces/Object3D.h"
#include "Objects/DebugAxisPlaneNode.h"
#include "External/SoundMini.h"

static SoundManager* s = nullptr;
static RenderHelper* rh = nullptr;
static lua_State* l = nullptr;

SoundSource::SoundSource() {
	minDist = s->getDefaultMin();
	maxDist = s->getDefaultMax();
}

SoundSource::SoundSource(const SoundSource& o) : SoundSource() {
	src = o.src;
}

SoundSource::SoundSource(const std::string& path, int type) {
	loadFromFile(path, type);
}

bool SoundSource::play(bool td) {
	if (!src) return false;
	if (cur)
		cur->stop();

	cur = s->play(src, td, loops);
	if (cur && td)
		ma_sound_set_position(&cur->sound, pos.x, pos.y, -pos.z);
	if (cur) {
		ma_sound_set_min_distance(&cur->sound, minDist);
		ma_sound_set_max_distance(&cur->sound, maxDist);
		ma_sound_set_volume(&cur->sound, vol);
		cur->setPaused(false);
	}
	is3D = td;

	return cur != nullptr;
}

void SoundSource::stop() {
	if (!src || !cur) return;
	cur->stop();
	parent = nullptr;
	is3D = false;
}

bool SoundSource::isPlaying() {
	if (!src || !cur) return false;
	return !cur->isFinished();
}

bool SoundSource::getPaused() {
	if (!src || !cur) return false;
	return cur->paused;
}

void SoundSource::setPaused(bool v) {
	if (!src || !cur) return;
	cur->setPaused(v);
}

bool SoundSource::getLooping() {
	return cur ? ma_sound_is_looping(&cur->sound) != MA_FALSE : loops;
}

void SoundSource::setLooping(bool v) {
	loops = v;
	if (cur) ma_sound_set_looping(&cur->sound, v);
}

float SoundSource::getMinDist() {
	return cur ? ma_sound_get_min_distance(&cur->sound) : 0.0f;
}

void SoundSource::setMinDist(float f) {
	if (!src || !cur) return;
	ma_sound_set_min_distance(&cur->sound, f);
	minDist = f;
}

float SoundSource::getMaxDist() {
	return cur ? ma_sound_get_max_distance(&cur->sound) : 0.0f;
}

void SoundSource::setMaxDist(float f) {
	if (!src || !cur) return;
	ma_sound_set_max_distance(&cur->sound, f);
	maxDist = f;
}

void SoundSource::setVolume(float f) {
	if (!src) return;
	vol = f / 100.0f;
	if (cur) ma_sound_set_volume(&cur->sound, vol);
}

float SoundSource::getVolume() {
	return (cur ? ma_sound_get_volume(&cur->sound) : vol) * 100.0f;
}

void SoundSource::setPitch(float f) {
	if (!src || !cur) return;
	ma_sound_set_pitch(&cur->sound, f > 0.01f ? f : 0.0f);
}

float SoundSource::getPitch() {
	return cur ? ma_sound_get_pitch(&cur->sound) : 0.0f;
}

void SoundSource::setPan(float f) {
	if (!src || !cur) return;
	ma_sound_set_pan(&cur->sound, f);
}

float SoundSource::getPan() {
	return cur ? ma_sound_get_pan(&cur->sound) : 0.0f;
}

int SoundSource::getPlayPosition() {
	if (!cur) return 0;
	float sec = 0.0f;
	if (ma_sound_get_cursor_in_seconds(&cur->sound, &sec) != MA_SUCCESS) return -1;
	return (int)(sec * 1000.0f);
}

void SoundSource::setPlayPosition(int ms) {
	if (!src || !cur) return;
	ma_sound_seek_to_second(&cur->sound, ms / 1000.0f);
}

int SoundSource::getPlayLength() {
	if (!cur) return 0;
	float sec = 0.0f;
	if (ma_sound_get_length_in_seconds(&cur->sound, &sec) != MA_SUCCESS) return -1;
	return (int)(sec * 1000.0f);
}

void SoundSource::setVelocity(const Vec3& v) {
	if (!src || !cur) return;
	vel = Vec3S{ v.getX(), v.getY(), v.getZ() };
	float f = s->getDistanceFactor();
	ma_sound_set_velocity(&cur->sound, vel.x * f, vel.y * f, -vel.z * f);
}

Vec3 SoundSource::getVelocity() {
	return cur ? Vec3(vel.x, vel.y, vel.z) : Vec3();
}

void SoundSource::setPosition(const Vec3& p) {
	pos.x = p.getX();
	pos.y = p.getY();
	pos.z = p.getZ();

	if (!src || !cur) return;
	ma_sound_set_position(&cur->sound, pos.x, pos.y, -pos.z);

	if (dVisual && dAxis) {
		irr::core::vector3df out;
		if (parent)
			out = parent->getAbsolutePosition();
		else
			out = irr::core::vector3df(pos.x, pos.y, pos.z);
		dVisual->setPosition(out);
		dAxis->setPosition(out);
	}
}

Vec3 SoundSource::getPosition() {
	return cur ? Vec3(pos.x, pos.y, pos.z) : Vec3();
}

bool SoundSource::getDebug() {
	return cur && is3D && dVisual;
}

void SoundSource::setDebug(bool v) {
	if (v) {
		if (!is3D || !cur) return;
		if (dVisual) { dVisual->remove(); dVisual = nullptr; }
		dVisual = rh->createDebugNode(DEBUG3D_TYPE::SOUND);
		dAxis = new DebugAxisPlaneNode(dVisual->getSceneManager()->getRootSceneNode(), dVisual->getSceneManager());
		dAxis->drop();
		setPosition(getPosition());
	} else {
		if (dVisual) {
			dVisual->remove();
			dVisual = nullptr;
		}
		if (dAxis) {
			dVisual->remove();
			dAxis = nullptr;
		}
	}
}

bool SoundSource::attachTo(sol::optional<Object3D*> p) {
	if (!cur || !is3D) return false;

	if (!p || *p == nullptr) {
		parent = nullptr; // SoundManager update will resolve
		s->detachSoundFromNode(cur.get());
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
	return src ? src->path : "";
}

void SoundSource::collected() {
	if (src) s->warnGarbageCollection(src->path);
}

sol::object SoundSource::destroy() {
	if (cur) {
		s->detachSoundFromNode(cur.get());
		cur->stop();
	}
	cur = nullptr;
	src = nullptr;
	return sol::make_object(l, sol::nil);
}

sol::object SoundSource::purge() {
	if (cur) {
		s->detachSoundFromNode(cur.get());
		cur->stop();
	}
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
	if (cur) cur->clearEffects();
}

bool SoundSource::addEchoEffect(float wetDry, float feedback, float delay) {
	if (!cur) return false;
	return cur->setEffect(SoundEffectType::Echo, makeEchoEffect(cur->engine, wetDry, feedback, delay));
}

bool SoundSource::addReverbEffect(float inputGain, float mix, float time, float freqRatio) {
	if (!cur) return false;
	return cur->setEffect(SoundEffectType::Reverb, makeReverbEffect(cur->engine, inputGain, mix, time, freqRatio));
}

bool SoundSource::addParamEqEffect(float fCenter, float fBandwidth, float fGain) {
	if (!cur) return false;
	return cur->setEffect(SoundEffectType::ParamEq, makeParamEqEffect(cur->engine, fCenter, fBandwidth, fGain));
}

void Object::SoundSourceBind::bind(lua_State* ls, SoundManager* sou, RenderHelper* renh) {
	l = ls;
	s = sou;
	rh = renh;

	// Object Sound, A source of sound, whether that be for sound effects or music.

	// Constructor
	// Constructor Sound
	// Constructor string path, Lime.Enum.SoundType? type

	sol::state_view view(ls);
	sol::usertype<SoundSource> obj = view.new_usertype<SoundSource>(
		"Sound",
		"new", sol::factories(
			[]() { return std::make_shared<SoundSource>(); },
			[](const SoundSource& s) { return std::make_shared<SoundSource>(s); },
			[](const std::string& p) { return std::make_shared<SoundSource>(p); },
			[](const std::string& p, int t) { return std::make_shared<SoundSource>(p, t); }
		),

		sol::meta_function::type, [](const SoundSource&) { return "Sound"; },
		sol::meta_function::garbage_collect, [](SoundSource& ss) { ss.collected(); },

		// Field boolean paused, Whether or not this `Sound` is paused.
		"paused", sol::property(&SoundSource::getPaused, &SoundSource::setPaused),

		// Field boolean looping, Whether or not this `Sound` loops on playback. 
		"looping", sol::property(&SoundSource::getLooping, &SoundSource::setLooping),

		// Field number volume, The volume of this `Sound`.
		"volume", sol::property(&SoundSource::getVolume, &SoundSource::setVolume),

		// Field number speed, The playback speed of this `Sound`.
		"speed", sol::property(&SoundSource::getPitch, &SoundSource::setPitch),

		// Field number pan, The pan of this `Sound`, where -1.0 is left and 1.0 is right. 
		"pan", sol::property(&SoundSource::getPan, &SoundSource::setPan),

		// Field number minimumDistance, Sets the minimum listening distance for this `Sound`. Only applicable if this object is played in 3D.
		"minimumDistance", sol::property(&SoundSource::getMinDist, &SoundSource::setMinDist),

		// Field number maximumDistance, Sets the maximum listening distance for this `Sound`. Only applicable if this object is played in 3D.
		"maximumDistance", sol::property(&SoundSource::getMaxDist, &SoundSource::setMaxDist),

		// Field number playbackPosition, The current playback position of this `Sound`.
		"playbackPosition", sol::property(&SoundSource::getPlayPosition, &SoundSource::setPlayPosition),

		// Field Vec3 velocity, The velocity of this `Sound`. Only applicable if this object is played in 3D.
		"velocity", sol::property(
			[](SoundSource& c) { return Vec3( [&c]{ return c.getVelocity(); }, [&c](const Vec3& v){ c.setVelocity(v); } ); },
			[](SoundSource& c, const Vec3& v) { c.setVelocity(v); }
		),

		// Field Vec3 position, The position of this `Sound` in the scene. Only applicable if this `Sound` is played in 3D.
		"position", sol::property(
			[](SoundSource& c) { return Vec3( [&c]{ return c.getPosition(); }, [&c](const Vec3& v){ c.setPosition(v); } ); },
			[](SoundSource& c, const Vec3& v) { c.setPosition(v); }
		),

		// Field boolean debug, Show debug information about this object in the scene.
		"debug", sol::property(&SoundSource::getDebug, &SoundSource::setDebug)
	);

	obj[sol::meta_function::to_string] = [](const SoundSource& v) {
		return "Sound";
		};

	// Play this `Sound`.
	// Params boolean? is3D
	// Returns boolean
	obj.set_function("play", &SoundSource::play);

	// Stop this `Sound`.
	// Returns void
	obj.set_function("stop", &SoundSource::stop);

	// Returns true if this `Sound` is playing.
	// Returns boolean
	obj.set_function("isPlaying", &SoundSource::isPlaying);

	// Returns the playback length of this `Sound`.
	// Returns number
	obj.set_function("getLength", &SoundSource::getPlayLength);

	// Returns the file path of the sound loaded into this `Sound`.
	// Returns string
	obj.set_function("getPath", &SoundSource::getPath);

	// Parents this `Sound` to a 3D object. (NOTE: This `Sound` must be playing in 3D)
	// Params any parent
	// Returns boolean
	obj.set_function("parentTo", &SoundSource::attachTo);

	// Returns true if this `Sound` is parented to a 3D object.
	// Returns boolean
	obj.set_function("hasParent", &SoundSource::isAttached);

	// Loads a new sound into this `Sound`. (WARNING: Unused sounds should be purged to free up unused memory)
	// Params string path, Lime.Enum.SoundType? type
	// Returns boolean
	obj.set_function("load", &SoundSource::loadFromFile);

	// Destroys this `Sound`, which stops itself from playing in the scene as well as detaching from a parent 3D object. To free this sound from memory, see `Sound:purge`.
	// Returns nil
	obj.set_function("destroy", &SoundSource::destroy);

	// Purges this `Sound`, effectively removing it from memory. If other `Sound` objects use this sound, there may be issues.
	// Returns nil
	obj.set_function("purge", &SoundSource::purge);

	// Clears all effects applied to this `Sound`. Stopping or destroying this `Sound` will clear its effects.
	// Returns void
	obj.set_function("clearEffects", &SoundSource::clearEffects);

	// Enables echoing on this `Sound`. Only applicable if this `Sound` is playing. This effect repeats the sound with decay over time.
	// Params
	// Params number wetDry, number feedback, number delayMs
	// Returns bool
	obj.set_function("addEchoEffect", &SoundSource::addEchoEffect);

	// Enables reverb on this `Sound`. Only applicable if this `Sound` is playing. This effect mixes the sound to bounce off surfaces in a room or a cave.
	// Params
	// Params number inputGain, number mix, number timeMs, number freqRatio
	// Returns bool
	obj.set_function("addReverbEffect", &SoundSource::addReverbEffect);

	// Enables parametric equilization on this `Sound`. Only applicable if this `Sound` is playing. This effect amplifies or attenuates signals at a given frequency.
	// Params
	// Params number threshold, number ratio
	// Returns bool
	obj.set_function("addParamEqEffect", &SoundSource::addParamEqEffect);

	// End Object
}