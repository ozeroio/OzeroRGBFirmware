#ifndef OZERO_RGB_BREATHING_H
#define OZERO_RGB_BREATHING_H

#define BREATHING_WAVE_TRIANGLE 0
#define BREATHING_WAVE_CIRCLE 1
#define BREATHING_WAVE_GAUSSIAN 2
#define BREATHING_WAVE_LENGTH 256

#include <config.h>
#include <effects/effect.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint32_t wave;
	uint32_t beta;
	uint32_t gama;
	uint32_t delay;
	uint8_t randomize;
	uint8_t velocity;
} BreathingEffectState;

class BreathingEffect : public virtual Effect {

protected:
	static uint8_t precomputedWave[BREATHING_WAVE_LENGTH];

	static uint8_t position;

	static BreathingEffect *instance;

	BreathingEffect();

public:
	BreathingEffect(BreathingEffect &other) = delete;

	void operator=(const BreathingEffect &) = delete;

	static BreathingEffect *getInstance();

	void setup() override;

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};

#endif// OZERO_RGB_BREATHING_H
