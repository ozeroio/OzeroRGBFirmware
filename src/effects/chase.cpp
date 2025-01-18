#include "chase.h"
#include <strip.h>

ChaseEffect *ChaseEffect::instance = nullptr;
uint8_t ChaseEffect::position = 0;

ChaseEffect::ChaseEffect() : Task(EffectCode::CHASE_EFFECT, new ChaseEffectState(), new ChaseEffectState()) {
}

ChaseEffect *ChaseEffect::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new ChaseEffect();
	}
	return instance;
}

uint32_t ChaseEffect::loop() {
	auto strip = Strip::getInstance();
	auto state = static_cast<ChaseEffectState *>(greenState);

	position += state->invertDir ? 1 : -1;
	for (uint32_t i = 0; i < NUM_LEDS; i += state->fillLen + state->gapLen) {
		for (uint32_t j = 0; j < state->fillLen; j++) {
			strip->setPixel((i + position + j) % NUM_LEDS, state->red, state->green, state->blue);
		}
		for (uint32_t j = state->fillLen; j < state->fillLen + state->gapLen; j++) {
			strip->setPixel((i + position + j) % NUM_LEDS, 0, 0, 0);
		}
	}
	Strip::showStrip();

	return state->delay;
}

uint32_t ChaseEffect::getSerializationSize() {
	return sizeof(ChaseEffectState);
}

void ChaseEffect::serialize(RandomAccess *randomAccess) {
	auto state = static_cast<ChaseEffectState *>(greenState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->red);
	randomAccess->writeUnsignedChar(state->green);
	randomAccess->writeUnsignedChar(state->blue);
	randomAccess->writeUnsignedInt(state->delay);
	randomAccess->writeUnsignedChar(state->invertDir);
	randomAccess->writeUnsignedInt(state->gapLen);
	randomAccess->writeUnsignedInt(state->fillLen);
	xSemaphoreGive(semaphore);
}

void ChaseEffect::deserialize(RandomAccess *randomAccess) {
	auto state = static_cast<ChaseEffectState *>(blueState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->red = randomAccess->readUnsignedChar();
	state->green = randomAccess->readUnsignedChar();
	state->blue = randomAccess->readUnsignedChar();
	state->delay = randomAccess->readUnsignedInt();
	state->invertDir = randomAccess->readUnsignedChar();
	state->gapLen = randomAccess->readUnsignedInt();
	state->fillLen = randomAccess->readUnsignedInt();
	xSemaphoreGive(semaphore);
}
