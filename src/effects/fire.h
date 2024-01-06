#ifndef OZERO_RGB_FIRE_H
#define OZERO_RGB_FIRE_H

#include <config.h>
#include <effects/effect.h>

#define FIRE_MAX_NUM_SEGMENTS 8
#define FIRE_MIN_SEGMENT_SIZE 4

typedef struct {
	uint32_t start;
	uint32_t length;
	uint8_t inverted;
	uint8_t sparking;
	uint8_t cooling;
	uint8_t sparkLevel;
} FireEffectSegment;

typedef struct {
	uint8_t numSegments;
	uint8_t delay;
	FireEffectSegment **segments;
} FireEffectState;

class FireEffect : public virtual Effect {

protected:
	static FireEffect *instance;

	FireEffect();

	static void freeStateMemory(FireEffectState *state);

	static FireEffectState *mallocStateMemory(uint8_t numSegments);

public:
	static uint8_t heat[NUM_LEDS];

	FireEffect(FireEffect &other) = delete;

	void operator=(const FireEffect &) = delete;

	static FireEffect *getInstance();

	void setup() override;

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};

#endif// OZERO_RGB_FIRE_H
