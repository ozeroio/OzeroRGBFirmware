#ifndef OZERO_RGB_CHASE_H
#define OZERO_RGB_CHASE_H

#include <config.h>
#include <effects/effect.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint32_t delay;
	uint8_t invertDir;
	uint32_t gapLen;
	uint32_t fillLen;
} ChaseEffectState;

class ChaseEffect : public virtual Effect {

protected:
	static uint8_t position;

	static ChaseEffect *instance;

	ChaseEffect();

public:
	ChaseEffect(ChaseEffect &other) = delete;

	void operator=(const ChaseEffect &) = delete;

	static ChaseEffect *getInstance();

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};

#endif// OZERO_RGB_CHASE_H
