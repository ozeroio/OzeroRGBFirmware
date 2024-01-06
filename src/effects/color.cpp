#include "color.h"
#include <strip.h>

ColorEffect *ColorEffect::instance = nullptr;

ColorEffect::ColorEffect() : Task(EffectCode::COLOR_EFFECT, new ColorEffectState(), new ColorEffectState()) {
}

ColorEffect *ColorEffect::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new ColorEffect();
	}
	return instance;
}

void ColorEffect::setup() {
	Effect::setup();

	auto strip = Strip::getInstance();
	auto state = (ColorEffectState *) greenState;

	if (state->randomize) {
		for (int32_t i = 0; i < Strip::numLeds(); i++) {
			strip->setPixel(i, random(255), random(255), random(255));
		}
	} else {
		strip->setStrip(state->red, state->green, state->blue);
	}
	Strip::showStrip();
}

uint32_t ColorEffect::loop() {

	// Nothing to do on loop.
	// Delay can be as big as possible, processing task will be notified if it needs to finish up.
	return 1000;
}

uint32_t ColorEffect::getSerializationSize() {
	return sizeof(ColorEffectState);
}

void ColorEffect::serialize(RandomAccess *randomAccess) {
	auto state = (ColorEffectState *) greenState;
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->red);
	randomAccess->writeUnsignedChar(state->green);
	randomAccess->writeUnsignedChar(state->blue);
	randomAccess->writeUnsignedChar(state->randomize);
	xSemaphoreGive(semaphore);
}

void ColorEffect::deserialize(RandomAccess *randomAccess) {
	auto state = (ColorEffectState *) blueState;
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->red = randomAccess->readUnsignedChar();
	state->green = randomAccess->readUnsignedChar();
	state->blue = randomAccess->readUnsignedChar();
	state->randomize = randomAccess->readUnsignedChar();
	xSemaphoreGive(semaphore);
}
