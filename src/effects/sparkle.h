#ifndef OZERO_RGB_SPARKLE_H
#define OZERO_RGB_SPARKLE_H

#include <config.h>
#include <effects/effect.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint32_t numLeds;
	uint32_t delay;
	uint8_t randomize;
} SparkleEffectState;

class SparkleEffect : public virtual Effect {

protected:
	static SparkleEffect *instance;

	SparkleEffect();

public:
	SparkleEffect(SparkleEffect &other) = delete;

	void operator=(const SparkleEffect &) = delete;

	static SparkleEffect *getInstance();

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};


#endif// OZERO_RGB_SPARKLE_H
