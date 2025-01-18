#include "breathing.h"
#include <device.h>
#include <strip.h>

BreathingEffect *BreathingEffect::instance = nullptr;
uint8_t BreathingEffect::position = 0;
uint8_t BreathingEffect::precomputedWave[256] = {};

BreathingEffect::BreathingEffect() : Task(EffectCode::BREATHING_EFFECT, new BreathingEffectState(), new BreathingEffectState()) {
}

BreathingEffect *BreathingEffect::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new BreathingEffect();
	}
	return instance;
}

void BreathingEffect::setup() {
	Effect::setup();

	auto strip = Strip::getInstance();
	auto state = static_cast<BreathingEffectState *>(greenState);

	auto brightness = Device::getInstance()->getBrightness();

	position = 0;
	auto beta = (float) (state->beta / 255.0);
	auto gama = (float) (state->gama / 255.0);
	for (uint8_t x = 0; x < 255; x++) {
		switch (state->wave) {
			case BREATHING_WAVE_TRIANGLE:
				precomputedWave[x] = uint8_t(brightness * (1.0 - abs((2.0 * (x / 255.0)) - 1.0)));
				break;
			case BREATHING_WAVE_CIRCLE:
				precomputedWave[x] = uint8_t(brightness * sqrt(1.0 - pow(abs((2.0 * (x / 255.0)) - 1.0), 2.0)));
				break;
			case BREATHING_WAVE_GAUSSIAN:
				precomputedWave[x] = uint8_t(brightness * exp(-(pow(((x / 255.0) - beta) / gama, 2.0)) / 2.0));
				break;
		}
	}
	if (state->randomize) {
		for (int32_t p = 0; p < Strip::numLeds(); p++) {
			strip->setPixel(p, random(255), random(255), random(255));
		}
	} else {
		strip->setStrip(state->red, state->green, state->blue);
	}
}

uint32_t BreathingEffect::loop() {
	auto state = static_cast<BreathingEffectState *>(greenState);

	FastLED.setBrightness(precomputedWave[position += state->velocity]);
	Strip::showStrip();

	return state->delay;
}

uint32_t BreathingEffect::getSerializationSize() {
	return sizeof(BreathingEffectState);
}

void BreathingEffect::serialize(RandomAccess *randomAccess) {
	auto state = static_cast<BreathingEffectState *>(greenState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->red);
	randomAccess->writeUnsignedChar(state->green);
	randomAccess->writeUnsignedChar(state->blue);
	randomAccess->writeUnsignedInt(state->wave);
	randomAccess->writeUnsignedInt(state->beta);
	randomAccess->writeUnsignedInt(state->gama);
	randomAccess->writeUnsignedInt(state->delay);
	randomAccess->writeUnsignedChar(state->randomize);
	randomAccess->writeUnsignedInt(state->velocity);
	xSemaphoreGive(semaphore);
}

void BreathingEffect::deserialize(RandomAccess *randomAccess) {
	auto state = static_cast<BreathingEffectState *>(blueState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->red = randomAccess->readUnsignedChar();
	state->green = randomAccess->readUnsignedChar();
	state->blue = randomAccess->readUnsignedChar();
	state->wave = randomAccess->readUnsignedInt();
	state->beta = randomAccess->readUnsignedInt();
	state->gama = randomAccess->readUnsignedInt();
	state->delay = randomAccess->readUnsignedInt();
	state->randomize = randomAccess->readUnsignedChar();
	state->velocity = randomAccess->readUnsignedInt();
	xSemaphoreGive(semaphore);
}
