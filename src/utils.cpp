#include "utils.h"
#include <FastLED.h>
#include <strip.h>

void Utils::ensureLimits() {
}

void Utils::shiftRight(uint32_t n) {
	auto strip = Strip::getInstance();
	auto numLeds = Strip::numLeds();
	auto size = sizeof(CRGB);
	n = min(n, uint32_t(numLeds));
	memcpy(&strip->bufferBlue[n], strip->bufferGreen, size * (numLeds - n));
	memcpy(&strip->bufferBlue[0], &strip->bufferGreen[numLeds - n], size * n);
	strip->swapBuffer();
}

void Utils::shiftLeft(uint32_t n) {
	auto strip = Strip::getInstance();
	auto numLeds = Strip::numLeds();
	auto size = sizeof(strip->bufferGreen[0]);
	n = min(n, uint32_t(numLeds));
	memcpy(strip->bufferBlue, &strip->bufferGreen[n], size * (numLeds - n));
	memcpy(&strip->bufferBlue[numLeds - n], &strip->bufferGreen[0], size * n);
	strip->swapBuffer();
}
