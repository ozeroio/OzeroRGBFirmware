#ifndef OZERO_STRIP_H
#define OZERO_STRIP_H

#include <FastLED.h>
#include <config.h>

#define BUFFER_SIZE (NUM_LEDS * sizeof(CRGB))

class Strip {

protected:
	static Strip *instance;

	Strip();

public:
	CRGB *bufferGreen;
	CRGB *bufferBlue;

	Strip(Strip &other) = delete;

	void operator=(const Strip &) = delete;

	static Strip *getInstance();

	void setup() const {
		CFastLED::addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(bufferGreen, NUM_LEDS, 0).setCorrection(TypicalLEDStrip);
	}

	void setPixel(uint32_t pixel, uint8_t red, uint8_t green, uint8_t blue) const;

	void setStrip(uint8_t red, uint8_t green, uint8_t blue) const;

	inline void clearStrip() const {
		setStrip(0, 0, 0);
	}

	inline static void setBrightness(uint8_t brightness) {
		FastLED.setBrightness(brightness);
	}

	inline static void showStrip() {
		FastLED.show();
	}

	inline static int32_t numLeds() {
		return NUM_LEDS;
	}

	inline static int32_t halfNumLeds() {
		return HALF_NUM_LEDS;
	}

	inline CRGB getLastLed() const {
		return bufferGreen[NUM_LEDS - 1];
	}

	inline void swapBuffer() const {
		memcpy(bufferGreen, bufferBlue, BUFFER_SIZE);
	}
};

#endif// OZERO_STRIP_H
