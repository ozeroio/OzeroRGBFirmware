#include "device.h"
#include <persistence.h>


Device *Device::instance = nullptr;

Device *Device::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new Device();
	}
	return instance;
}

DeviceState *Device::getState() const {
	return greenState;
}

Task *Device::getTask() const {
	return greenTask;
}

void Device::swapState() {
	xSemaphoreTake(semaphore, portMAX_DELAY);
	auto tempState = blueState;
	auto tempEffect = blueTask;
	blueState = greenState;
	greenState = tempState;
	blueTask = greenTask;
	greenTask = tempEffect;
	if (greenTask != nullptr) {
		greenTask->swapState();
	}
	xSemaphoreGive(semaphore);
}

void Device::setup() {
	deserialize(Persistence::getInstance()->getRandomAccess());
}

uint32_t Device::getSerializationSize() const {

	// NOTE: It might introduce padding: https://www.geeksforgeeks.org/is-sizeof-for-a-struct-equal-to-the-sum-of-sizeof-of-each-member/.
	uint32_t size = sizeof(DeviceState);
	if (greenTask != nullptr) {
		size += greenTask->getSerializationSize();
	}
	return size;
}

void Device::serialize(RandomAccess *randomAccess) const {
	xSemaphoreTake(semaphore, portMAX_DELAY);
	randomAccess->writeUnsignedInt(greenState->flags.value);
	randomAccess->writeUnsignedChar(greenState->brightness);
	randomAccess->writeUnsignedInt(greenState->task);
	if (greenTask != nullptr) {
		greenTask->serialize(randomAccess);
	}
	xSemaphoreGive(semaphore);
}

void Device::deserialize(RandomAccess *randomAccess) {
	xSemaphoreTake(semaphore, portMAX_DELAY);
	blueState->flags.value = randomAccess->readUnsignedInt();
	blueState->brightness = randomAccess->readUnsignedChar();
	blueState->task = randomAccess->readUnsignedInt();
	blueTask = Task::fetchRegisteredEffect(blueState->task);
	if (blueTask != nullptr) {
		blueTask->deserialize(randomAccess);
	}
	xSemaphoreGive(semaphore);
}
