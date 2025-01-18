#include "shift.h"
#include <strip.h>
#include <utils.h>

ShiftEffect *ShiftEffect::instance = nullptr;
volatile uint32_t ShiftEffect::position = 0;

ShiftEffect::ShiftEffect() : shiftType(0), Task(EffectCode::SHIFT_EFFECT,
												new ShiftEffectState(),
												new ShiftEffectState()) {
}

ShiftEffect *ShiftEffect::getInstance() {
	if (instance == nullptr) {
		instance = new ShiftEffect();
	}
	return instance;
}

uint32_t ShiftEffect::segmentSize() {

	// Length + a color struct
	return sizeof(uint32_t) + sizeof(EffectStateColor);
}

void ShiftEffect::freeStateMemory(ShiftEffectState *state) {
	for (int i = 0; i < state->numSegments; i++) {
		vPortFree(state->segments[i]->color);
		vPortFree(state->segments[i]);
	}
	vPortFree(state->segments);
	vPortFree(state);
}

ShiftEffectState *ShiftEffect::mallocStateMemory(uint8_t numSegments) {
	auto *state = new ShiftEffectState();
	state->segments = static_cast<ShiftEffectStateSegment **>(
			pvPortMalloc(numSegments * sizeof(ShiftEffectStateSegment *)));
	for (int i = 0; i < numSegments; i++) {
		auto segment = new ShiftEffectStateSegment();
		segment->color = new EffectStateColor();
		state->segments[i] = segment;
	}
	return state;
}

void ShiftEffect::setup() {
	Effect::setup();
	auto strip = Strip::getInstance();
	auto state = static_cast<ShiftEffectState *>(greenState);
	auto numLeds = Strip::numLeds();
	position = 0;
	shiftType = 0;
	uint32_t index = 0;
	while (index < numLeds) {
		for (int i = 0; i < state->numSegments; i++) {
			auto segment = state->segments[i];
			auto remaining = min(uint32_t(segment->length), uint32_t(numLeds - index));
			for (int j = 0; j < remaining; j++) {
				strip->setPixel(index++, segment->color->red, segment->color->green, segment->color->blue);
			}
		}

		// If index didn't change (infinity loop) or if we should not repeat. Break!
		if (!state->repeat || index == 0) {
			break;
		}
	}
	Strip::showStrip();
}

uint32_t ShiftEffect::loop() {
	auto state = static_cast<ShiftEffectState *>(greenState);
	uint8_t currentShiftType = shiftType;
	if (state->shiftType == SHIFT_BOUNCY_SHIFT_TYPE) {
		position += state->skipping;
		if (position >= state->bounceEvery) {
			shiftType = (shiftType == SHIFT_RIGHT_SHIFT_TYPE) ? SHIFT_LEFT_SHIFT_TYPE : SHIFT_RIGHT_SHIFT_TYPE;
			position = 0;
		}
	} else {
		currentShiftType = state->shiftType;
	}

	if (currentShiftType == SHIFT_RIGHT_SHIFT_TYPE) {
		Utils::shiftRight(state->skipping);
	} else if (currentShiftType == SHIFT_LEFT_SHIFT_TYPE) {
		Utils::shiftLeft(state->skipping);
	}

	Strip::showStrip();

	return state->delay;
}

uint32_t ShiftEffect::getSerializationSize() {
	auto state = static_cast<ShiftEffectState *>(greenState);

	// numSegments * its size.
	auto segmentsSize = state->numSegments * segmentSize();

	return sizeof(uint8_t) + // state->numSegments
		   sizeof(uint8_t) + // state->shiftType
		   sizeof(uint32_t) +// state->delay
		   sizeof(uint32_t) +// state->bounceEvery
		   sizeof(uint32_t) +// state->skipping
		   sizeof(uint8_t) + // state->repeat
		   segmentsSize;
}

void ShiftEffect::serialize(RandomAccess *randomAccess) {
	auto state = static_cast<ShiftEffectState *>(greenState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedChar(state->numSegments);
	for (int i = 0; i < state->numSegments; i++) {
		auto segment = state->segments[i];
		randomAccess->writeUnsignedInt(segment->length);
		randomAccess->writeUnsignedChar(segment->color->red);
		randomAccess->writeUnsignedChar(segment->color->green);
		randomAccess->writeUnsignedChar(segment->color->blue);
	}
	randomAccess->writeUnsignedChar(state->shiftType);
	randomAccess->writeUnsignedInt(state->delay);
	randomAccess->writeUnsignedInt(state->bounceEvery);
	randomAccess->writeUnsignedInt(state->skipping);
	randomAccess->writeUnsignedChar(state->repeat);
	xSemaphoreGive(semaphore);
}

void ShiftEffect::deserialize(RandomAccess *randomAccess) {
	auto numSegments = min(uint8_t(SHIFT_MAX_NUM_SEGMENTS), randomAccess->readUnsignedChar());
	freeStateMemory(static_cast<ShiftEffectState *>(blueState));
	blueState = mallocStateMemory(numSegments);
	auto state = static_cast<ShiftEffectState *>(blueState);
	xSemaphoreTake(semaphore, portMAX_DELAY);
	state->numSegments = numSegments;
	for (int i = 0; i < numSegments; i++) {
		auto segment = state->segments[i];
		segment->length = randomAccess->readUnsignedInt();
		segment->color->red = randomAccess->readUnsignedChar();
		segment->color->green = randomAccess->readUnsignedChar();
		segment->color->blue = randomAccess->readUnsignedChar();
	}
	state->shiftType = randomAccess->readUnsignedChar();
	state->delay = randomAccess->readUnsignedInt();
	state->bounceEvery = randomAccess->readUnsignedInt();
	state->skipping = randomAccess->readUnsignedInt();
	state->repeat = randomAccess->readUnsignedChar();
	xSemaphoreGive(semaphore);
}
