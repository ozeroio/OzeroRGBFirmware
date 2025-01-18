#include "fire.h"
#include <strip.h>

FireEffect *FireEffect::instance = nullptr;
uint8_t FireEffect::heat[] = {};

FireEffect::FireEffect() : Task(EffectCode::FIRE_EFFECT, new FireEffectState(), new FireEffectState()) {
}

FireEffect *FireEffect::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new FireEffect();
	}
	return instance;
}

void FireEffect::freeStateMemory(FireEffectState *state) {
	for (int i = 0; i < state->numSegments; i++) {
		vPortFree(state->segments[i]);
	}
	vPortFree(state->segments);
	vPortFree(state);
}

FireEffectState *FireEffect::mallocStateMemory(uint8_t numSegments) {
	auto *state = new FireEffectState();
	state->segments = static_cast<FireEffectSegment **>(
			pvPortMalloc(numSegments * sizeof(FireEffectSegment *)));
	for (int i = 0; i < numSegments; i++) {
		state->segments[i] = new FireEffectSegment();
	}
	return state;
}

void FireEffect::setup() {
	Effect::setup();
	for (uint8_t &i: heat) {
		i = 0;
	}
}

uint32_t FireEffect::loop() {
	auto strip = Strip::getInstance();
	auto state = static_cast<FireEffectState *>(greenState);

	for (int i = 0; i < state->numSegments; i++) {

		auto segment = state->segments[i];
		auto segmentEnd = segment->start + segment->length;

		if (segment->length < FIRE_MIN_SEGMENT_SIZE) {
			continue;
		}

		if (segmentEnd > Strip::numLeds()) {
			continue;
		}

		if (segment->sparkLevel >= segment->length) {
			continue;
		}

		// Max possible cool down amount for this iteration.
		auto maxCoolDown = ((segment->cooling * 10) / segment->length) + 2;

		// Step 1.  Cool down every cell a little
		// Direction doesn't matter.
		for (uint32_t j = segment->start; j < segmentEnd; j++) {
			heat[j] = qsub8(heat[j], random8(0, maxCoolDown));
		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		// Direction matters - but we keep and populate heat as nothing was inverted.
		for (uint32_t k = segmentEnd - 1; k >= segment->start + 3; k--) {
			heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 3]) / 3;
		}

		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		uint8_t spark = random8() < segment->sparking;
		if (spark) {
			uint8_t sparkBrightness = random8(160, 255);
			uint32_t pos = segment->start + random8(segment->sparkLevel);
			heat[pos] = qadd8(heat[pos], sparkBrightness);
		}

		// Step 4.  Map from heat cells to LED colors and update strip.
		// Direction matters - and needs to be handled.
		if (segment->inverted) {
			for (uint32_t k = 0, j = segment->start; j < segmentEnd; j++, k++) {
				strip->bufferGreen[segmentEnd - k] = HeatColor(heat[j]);
			}
		} else {
			for (uint32_t j = segment->start; j < segmentEnd; j++) {
				strip->bufferGreen[j] = HeatColor(heat[j]);
			}
		}
	}
	Strip::showStrip();
	return state->delay;
}

uint32_t FireEffect::getSerializationSize() {
	auto state = static_cast<FireEffectState *>(greenState);

	// numSegments * its size.
	// NOTE: It might introduce padding: https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/.
	auto segmentsSize = state->numSegments * sizeof(FireEffectSegment);
	return sizeof(uint8_t) +// state->numSegments
		   sizeof(uint8_t) +// state->delay
		   segmentsSize;
}

void FireEffect::serialize(RandomAccess *randomAccess) {
	auto state = static_cast<FireEffectState *>(greenState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->numSegments);
	for (int i = 0; i < state->numSegments; i++) {
		auto segment = state->segments[i];
		randomAccess->writeUnsignedInt(segment->start);
		randomAccess->writeUnsignedInt(segment->length);
		randomAccess->writeUnsignedChar(segment->inverted);
		randomAccess->writeUnsignedChar(segment->sparking);
		randomAccess->writeUnsignedChar(segment->cooling);
		randomAccess->writeUnsignedChar(segment->sparkLevel);
	}
	randomAccess->writeUnsignedChar(state->delay);
	xSemaphoreGive(semaphore);
}

void FireEffect::deserialize(RandomAccess *randomAccess) {
	auto numSegments = min(uint8_t(FIRE_MAX_NUM_SEGMENTS), randomAccess->readUnsignedChar());
	freeStateMemory(static_cast<FireEffectState *>(blueState));
	blueState = mallocStateMemory(numSegments);
	auto state = static_cast<FireEffectState *>(blueState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->numSegments = numSegments;
	for (int i = 0; i < numSegments; i++) {
		auto segment = state->segments[i];
		segment->start = randomAccess->readUnsignedInt();
		segment->length = randomAccess->readUnsignedInt();
		segment->inverted = randomAccess->readUnsignedChar();
		segment->sparking = randomAccess->readUnsignedChar();
		segment->cooling = randomAccess->readUnsignedChar();
		segment->sparkLevel = randomAccess->readUnsignedChar();
	}
	state->delay = randomAccess->readUnsignedChar();
	xSemaphoreGive(semaphore);
}
