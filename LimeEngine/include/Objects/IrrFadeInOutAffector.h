#pragma once
#include <irrlicht.h>
#include <cmath>

class CParticleFadeInOutAffector : public irr::scene::IParticleAffector {
public:
	CParticleFadeInOutAffector(irr::f32 amp) : Amp(amp ? irr::core::clamp(amp, 0.0f, 1.0f) : 1.0f) {}

	virtual void affect(irr::u32 now, irr::scene::SParticle* particlearray, irr::u32 count) override {
		if (!Enabled) return;

		static const irr::video::SColor zero(0, 0, 0, 0);
		for (irr::u32 i = 0; i < count; ++i) {
			const irr::u32 maxdiff = particlearray[i].endTime - particlearray[i].startTime;
			if (maxdiff == 0) continue;
			const irr::f32 t = (irr::f32)(now - particlearray[i].startTime) / maxdiff;
			const irr::f32 d = std::sin(irr::core::PI * t) * Amp;

			particlearray[i].color = particlearray[i].startColor.getInterpolated(zero, d);
		}
	}

	virtual irr::scene::E_PARTICLE_AFFECTOR_TYPE getType() const override {
		return irr::scene::EPAT_FADE_OUT;
	}

private:
	irr::f32 Amp;
};