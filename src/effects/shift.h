#ifndef OZERO_RGB_SHIFT_H
#define OZERO_RGB_SHIFT_H

#define SHIFT_NONE_SHIFT_TYPE 0
#define SHIFT_RIGHT_SHIFT_TYPE 1
#define SHIFT_LEFT_SHIFT_TYPE 2
#define SHIFT_BOUNCY_SHIFT_TYPE 3
#define SHIFT_MAX_NUM_SEGMENTS 16

#include <effects/effect.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} EffectStateColor;

typedef struct {
	uint32_t length;
	EffectStateColor *color;
} ShiftEffectStateSegment;

typedef struct {
	uint8_t numSegments;
	uint8_t shiftType;
	uint32_t delay;
	uint32_t bounceEvery;
	uint32_t skipping;
	uint8_t repeat;
	ShiftEffectStateSegment **segments;
} ShiftEffectState;

class ShiftEffect : public virtual Effect {

protected:
	static ShiftEffect *instance;

	volatile static uint32_t position;

	volatile uint8_t shiftType;

	ShiftEffect();

	inline static uint32_t segmentSize();

	static void freeStateMemory(ShiftEffectState *state);

	static ShiftEffectState *mallocStateMemory(uint8_t numSegments);

public:
	ShiftEffect(ShiftEffect &other) = delete;

	void operator=(const ShiftEffect &) = delete;

	static ShiftEffect *getInstance();

	void setup() override;

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};

#endif// OZERO_RGB_SHIFT_H
