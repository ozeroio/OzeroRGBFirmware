#ifndef OZERO_RGB_WAVE_H
#define OZERO_RGB_WAVE_H

#include <config.h>
#include <effects/effect.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint32_t delay;
	uint8_t invertDir;
} WaveEffectState;

class WaveEffect : public virtual Effect {

protected:
	static uint8_t position;

	static WaveEffect *instance;

	WaveEffect();

public:
	WaveEffect(WaveEffect &other) = delete;

	void operator=(const WaveEffect &) = delete;

	static WaveEffect *getInstance();

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};

#endif// OZERO_RGB_WAVE_H
