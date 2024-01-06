#include "strip.h"

Strip *Strip::instance = nullptr;

Strip::Strip() {
	bufferGreen = static_cast<CRGB *>(pvPortMalloc(BUFFER_SIZE));
	bufferBlue = static_cast<CRGB *>(pvPortMalloc(BUFFER_SIZE));
}

Strip *Strip::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new Strip();
	}
	return instance;
}

void Strip::setPixel(const uint32_t pixel, const uint8_t red, const uint8_t green, const uint8_t blue) const {
	if (pixel >= NUM_LEDS) {
		return;
	}
	bufferGreen[pixel].red = red;
	bufferGreen[pixel].green = green;
	bufferGreen[pixel].blue = blue;
}

void Strip::setStrip(const uint8_t red, const uint8_t green, const uint8_t blue) const {
	for (uint32_t i = 0; i < NUM_LEDS; i++) {
		setPixel(i, red, green, blue);
	}
}
