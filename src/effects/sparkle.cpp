#include "sparkle.h"
#include <strip.h>

SparkleEffect *SparkleEffect::instance = nullptr;

SparkleEffect::SparkleEffect() : Task(EffectCode::SPARKLE_EFFECT,
									  new SparkleEffectState(),
									  new SparkleEffectState()) {
}

SparkleEffect *SparkleEffect::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new SparkleEffect();
	}
	return instance;
}

uint32_t SparkleEffect::loop() {
	auto strip = Strip::getInstance();
	auto state = (SparkleEffectState *) greenState;

	// TODO: remove from here.
	// Why?
	int32_t pixels[state->numLeds];

	for (uint32_t i = 0; i < state->numLeds; i++) {
		pixels[i] = random(Strip::numLeds());
		strip->setPixel(pixels[i],
						state->randomize ? random(255) : state->red,
						state->randomize ? random(255) : state->green,
						state->randomize ? random(255) : state->blue);
	}
	Strip::showStrip();
	for (uint32_t i = 0; i < state->numLeds; i++) {
		strip->setPixel(pixels[i], 0, 0, 0);
	}

	return state->delay;
}

uint32_t SparkleEffect::getSerializationSize() {
	return sizeof(SparkleEffectState);
}

void SparkleEffect::serialize(RandomAccess *randomAccess) {
	auto state = (SparkleEffectState *) greenState;
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->red);
	randomAccess->writeUnsignedChar(state->green);
	randomAccess->writeUnsignedChar(state->blue);
	randomAccess->writeUnsignedInt(state->delay);
	randomAccess->writeUnsignedInt(state->numLeds);
	randomAccess->writeUnsignedChar(state->randomize);
	xSemaphoreGive(semaphore);
}

void SparkleEffect::deserialize(RandomAccess *randomAccess) {
	auto state = (SparkleEffectState *) blueState;
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->red = randomAccess->readUnsignedChar();
	state->green = randomAccess->readUnsignedChar();
	state->blue = randomAccess->readUnsignedChar();
	state->delay = randomAccess->readUnsignedInt();
	state->numLeds = randomAccess->readUnsignedInt();
	state->randomize = randomAccess->readUnsignedChar();
	xSemaphoreGive(semaphore);
}
