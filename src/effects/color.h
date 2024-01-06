#ifndef OZERO_RGB_COLOR_H
#define OZERO_RGB_COLOR_H

#include <effects/effect.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t randomize;
} ColorEffectState;

class ColorEffect : public virtual Effect {

protected:
	static ColorEffect *instance;

	ColorEffect();

public:
	ColorEffect(ColorEffect &other) = delete;

	void operator=(const ColorEffect &) = delete;

	static ColorEffect *getInstance();

	void setup() override;

	uint32_t loop() override;

	uint32_t getSerializationSize() override;

	void serialize(RandomAccess *randomAccess) override;

	void deserialize(RandomAccess *randomAccess) override;
};

#endif// OZERO_RGB_COLOR_H
