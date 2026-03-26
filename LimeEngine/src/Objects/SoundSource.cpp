#include "Objects/SoundSource.h"

#include "SoundManager.h"
#include "irrKlang.h"
#include <sol/sol.hpp>
#include "Objects/Vec3.h"

static SoundManager* s = nullptr;
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

	return cur;
}

void SoundSource::stop() {
	if (!src || !cur) return;
	cur->stop();
	cur->drop();
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

void SoundSource::setVolume(float f) {
	if (!src || !cur) return;
	cur->setVolume(f);
}

float SoundSource::getVolume() {
	return cur ? cur->getVolume() : 0.0f;
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

bool SoundSource::getDoSFX() {
	return doSFX;
}

void SoundSource::setDoSFX(bool v) {
	doSFX = v;
}

bool SoundSource::getDebug() {
	return false; // TODO
}

void SoundSource::setDebug(bool v) {
	// TODO
}

void SoundSource::attachTo(sol::optional<Object3D*> parent) {
}

std::string SoundSource::getPath() {
	return src ? src->getName() : "";
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
	src->drop();
	cur = nullptr;
	src = nullptr;

	src = s->createSoundSource(path, type);
	return src;
}

void Object::SoundSourceBind::bind(lua_State* ls, SoundManager* sou) {
	l = ls;
	s = sou;

	// Object SoundSource, A source of sound, whether that be for sound effects or music.

	// Constructor
	// Constructor string path, Lime.Enum.SoundType? type

	sol::state_view view(ls);
	sol::usertype<SoundSource> obj = view.new_usertype<SoundSource>(
		"SoundSource",
		sol::constructors<SoundSource(), SoundSource(const std::string&, int)>(),

		sol::meta_function::type, [](const SoundSource&) { return "SoundSource"; },

		// Field boolean paused, Whether or not this `SoundSource` is paused.
		"paused", sol::property(&SoundSource::getPaused, &SoundSource::setPaused),

		// Field boolean loops, Whether or not this `SoundSource` loops on playback. 
		"loops", sol::property(&SoundSource::getLooping, &SoundSource::setLooping),

		// Field number volume, The volume of this `SoundSource`.
		"volume", sol::property(&SoundSource::getVolume, &SoundSource::setVolume),

		// Field number speed, The playback speed of this `SoundSource`.
		"speed", sol::property(&SoundSource::getPitch, &SoundSource::setPitch),

		// Field number pan, The pan of this `SoundSource`, where -1.0 is left and 1.0 is right. 
		"pan", sol::property(&SoundSource::getPan, &SoundSource::setPan),

		// Field number playbackPosition, The current playback position of this `SoundSource`.
		"playbackPosition", sol::property(&SoundSource::getPlayPosition, &SoundSource::setPlayPosition),

		// Field Vec3 velocity, The velocity of this `SoundSource`. Only applicable if this object is played in 3D.
		"velocity", sol::property(
			[](SoundSource& c) { return Vec3{ [&] { return c.getVelocity(); }, [&](auto v) { c.setVelocity(v); } }; },
			[](SoundSource& c, const Vec3& v) { c.setVelocity(v); }
		),

		// Field boolean sfx, Whether or not sound effects are enabled on playback. This flag must be enabled, then the object should be played to see any effect.
		"sfx", sol::property(&SoundSource::getDoSFX, &SoundSource::setDoSFX),

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

	// End Object
}
