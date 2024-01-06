#include "wave.h"
#include <strip.h>

WaveEffect *WaveEffect::instance = nullptr;
uint8_t WaveEffect::position = 0;

WaveEffect::WaveEffect() : Task(EffectCode::WAVE_EFFECT, new WaveEffectState(), new WaveEffectState()) {
}

WaveEffect *WaveEffect::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new WaveEffect();
	}
	return instance;
}

uint32_t WaveEffect::loop() {
	auto strip = Strip::getInstance();
	auto state = (WaveEffectState *) greenState;

	position += state->invertDir ? 1 : -1;
	for (int i = 0; i < Strip::numLeds(); i++) {
		double level = (sin(i + position) * 127 + 128) / 255;
		strip->setPixel(i, uint8_t(level * state->red), uint8_t(level * state->green), uint8_t(level * state->blue));
	}
	Strip::showStrip();

	return state->delay;
}

uint32_t WaveEffect::getSerializationSize() {
	return sizeof(WaveEffectState);
}

void WaveEffect::serialize(RandomAccess *randomAccess) {
	auto state = (WaveEffectState *) greenState;
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->red);
	randomAccess->writeUnsignedChar(state->green);
	randomAccess->writeUnsignedChar(state->blue);
	randomAccess->writeUnsignedInt(state->delay);
	randomAccess->writeUnsignedChar(state->invertDir);
	xSemaphoreGive(semaphore);
}

void WaveEffect::deserialize(RandomAccess *randomAccess) {
	auto state = (WaveEffectState *) blueState;
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->red = randomAccess->readUnsignedChar();
	state->green = randomAccess->readUnsignedChar();
	state->blue = randomAccess->readUnsignedChar();
	state->delay = randomAccess->readUnsignedInt();
	state->invertDir = randomAccess->readUnsignedChar();
	xSemaphoreGive(semaphore);
}
