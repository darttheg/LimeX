#pragma once
#include <irrlicht.h>

class CParticleFadeInAffector : public irr::scene::IParticleAffector {
public:
	CParticleFadeInAffector(irr::u32 fadeInTime) : FadeInTime(fadeInTime ? (irr::f32)fadeInTime : 1.0f) {}

	virtual void affect(irr::u32 now, irr::scene::SParticle* particlearray, irr::u32 count) override {
		if (!Enabled) return;

		for (irr::u32 i = 0; i < count; ++i) {
			const irr::u32 elapsed = now - particlearray[i].startTime;
			if (elapsed < FadeInTime) {
				const irr::f32 d = 1.0f - (irr::f32)elapsed / FadeInTime;
				static const irr::video::SColor zero(0, 0, 0, 0);
				particlearray[i].color = zero.getInterpolated(particlearray[i].startColor, d);
			}
		}
	}

	virtual irr::scene::E_PARTICLE_AFFECTOR_TYPE getType() const override {
		return irr::scene::EPAT_FADE_OUT;
	}

private:
	irr::f32 FadeInTime;
};