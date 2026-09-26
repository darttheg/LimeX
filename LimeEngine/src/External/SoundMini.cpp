#include "External/SoundMini.h"
#include "extras/nodes/ma_reverb_node/ma_reverb_node.h"

#include <algorithm>
#include <cmath>

SoundMini::~SoundMini() {
	if (ready) ma_sound_uninit(&sound);
}

bool SoundMini::isFinished() {
	return !ready || stopped || ma_sound_at_end(&sound);
}

void SoundMini::setPaused(bool v) {
	paused = v;
	if (!ready || stopped) return;
	if (v) ma_sound_stop(&sound);
	else if (!ma_sound_at_end(&sound)) ma_sound_start(&sound);
}

void SoundMini::stop() {
	if (!ready) return;
	stopped = true;
	ma_sound_stop(&sound);
}

bool SoundMini::setEffect(SoundEffectType type, std::unique_ptr<SoundEffect> fx) {
	if (!ready || !fx) return false;

	std::unique_ptr<SoundEffect> old;
	auto it = std::find_if(effects.begin(), effects.end(), [type](const Slot& s) { return s.type == type; });
	if (it != effects.end()) {
		old = std::move(it->fx);
		it->fx = std::move(fx);
	} else {
		effects.push_back({ type, std::move(fx) });
	}

	relink();
	return true;
}

void SoundMini::clearEffects() {
	if (!ready) return;
	ma_node_attach_output_bus(&sound, 0, ma_engine_get_endpoint(engine), 0);
	effects.clear();
}

void SoundMini::relink() {
	ma_node* prev = &sound;
	for (auto& s : effects) {
		ma_node_attach_output_bus(prev, 0, s.fx->node(), 0);
		prev = s.fx->node();
	}
	ma_node_attach_output_bus(prev, 0, ma_engine_get_endpoint(engine), 0);
}

namespace {
	template <class T>
	struct NodeEffect : SoundEffect {
		T n{};
		void (*uninit)(T*) = nullptr;
		~NodeEffect() override { if (uninit) uninit(&n); }
		ma_node* node() override { return &n; }
	};

	float dbToLinear(float db) { return std::pow(10.0f, db / 20.0f); }
}

std::unique_ptr<SoundEffect> makeEchoEffect(ma_engine* e, float wetDry, float feedback, float delayMs) {
	ma_uint32 ch = ma_engine_get_channels(e);
	ma_uint32 sr = ma_engine_get_sample_rate(e);
	ma_uint32 frames = (ma_uint32)(std::clamp(delayMs, 1.0f, 2000.0f) * sr / 1000.0f);
	ma_delay_node_config c = ma_delay_node_config_init(ch, sr, frames, std::clamp(feedback, 0.0f, 100.0f) / 100.0f);
	c.delay.wet = std::clamp(wetDry, 0.0f, 100.0f) / 100.0f;
	c.delay.dry = 1.0f - c.delay.wet;
	auto fx = std::make_unique<NodeEffect<ma_delay_node>>();
	if (ma_delay_node_init(ma_engine_get_node_graph(e), &c, nullptr, &fx->n) != MA_SUCCESS) return nullptr;
	fx->uninit = [](ma_delay_node* p) { ma_delay_node_uninit(p, nullptr); };
	return fx;
}

std::unique_ptr<SoundEffect> makeParamEqEffect(ma_engine* e, float center, float bandwidth, float gainDb) {
	ma_uint32 ch = ma_engine_get_channels(e);
	ma_uint32 sr = ma_engine_get_sample_rate(e);
	// Bandwidth is in semitones
	double n = std::pow(2.0, std::clamp(bandwidth, 1.0f, 36.0f) / 12.0);
	double q = std::sqrt(n) / (n - 1.0);
	ma_peak_node_config c = ma_peak_node_config_init(ch, sr, std::clamp(gainDb, -15.0f, 15.0f), q, std::clamp(center, 80.0f, 16000.0f));
	auto fx = std::make_unique<NodeEffect<ma_peak_node>>();
	if (ma_peak_node_init(ma_engine_get_node_graph(e), &c, nullptr, &fx->n) != MA_SUCCESS) return nullptr;
	fx->uninit = [](ma_peak_node* p) { ma_peak_node_uninit(p, nullptr); };
	return fx;
}

std::unique_ptr<SoundEffect> makeReverbEffect(ma_engine* e, float inputGainDb, float mixDb, float timeMs, float freqRatio) {
	ma_uint32 ch = ma_engine_get_channels(e);
	if (ch > 2) return nullptr;
	ma_reverb_node_config c = ma_reverb_node_config_init(ch, ma_engine_get_sample_rate(e));
	auto fx = std::make_unique<NodeEffect<ma_reverb_node>>();
	if (ma_reverb_node_init(ma_engine_get_node_graph(e), &c, nullptr, &fx->n) != MA_SUCCESS) return nullptr;
	fx->uninit = [](ma_reverb_node* p) { ma_reverb_node_uninit(p, nullptr); };

	// ma_reverb_node_init ignores the config's room/wet/dry, so set them here
	float gain = dbToLinear(std::clamp(inputGainDb, -96.0f, 0.0f));
	float mix = dbToLinear(std::clamp(mixDb, -96.0f, 0.0f));
	verblib_set_room_size(&fx->n.reverb, std::clamp(timeMs / 3000.0f, 0.0f, 1.0f));
	verblib_set_damping(&fx->n.reverb, 1.0f - std::clamp(freqRatio, 0.001f, 0.999f));
	verblib_set_wet(&fx->n.reverb, gain * mix / verblib_scalewet);
	verblib_set_dry(&fx->n.reverb, gain / verblib_scaledry);
	return fx;
}