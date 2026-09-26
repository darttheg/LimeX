#pragma once

#include <memory>
#include <vector>
#include "miniaudio.h"

enum class SoundEffectType { Echo, Reverb, ParamEq };
struct SoundEffect {
	virtual ~SoundEffect() = default;
	virtual ma_node* node() = 0;
};

std::unique_ptr<SoundEffect> makeEchoEffect(ma_engine* e, float wetDry, float feedback, float delayMs);
std::unique_ptr<SoundEffect> makeReverbEffect(ma_engine* e, float inputGainDb, float mixDb, float timeMs, float freqRatio);
std::unique_ptr<SoundEffect> makeParamEqEffect(ma_engine* e, float center, float bandwidth, float gainDb);

struct SoundMini {
	ma_sound sound{};
	ma_engine* engine = nullptr;

	bool ready = false;
	bool stopped = false;
	bool paused = false;

	~SoundMini();

	bool isFinished();
	void setPaused(bool v);
	void stop();
	bool setEffect(SoundEffectType type, std::unique_ptr<SoundEffect> fx);
	void clearEffects();

private:
	struct Slot { SoundEffectType type; std::unique_ptr<SoundEffect> fx; };
	std::vector<Slot> effects;
	void relink();
};