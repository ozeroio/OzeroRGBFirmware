#ifndef OZERO_RGB_DEVICE_H
#define OZERO_RGB_DEVICE_H

#include <RandomAccess/RandomAccess.h>
#include <config.h>
#include <freertos/semphr.h>
#include <task.h>

typedef union {
	struct {
		uint32_t on : 1;
		uint32_t : 31;
	};
	uint32_t value;
} DeviceStateFlags;

typedef struct {
	DeviceStateFlags flags;
	uint8_t brightness;
	uint32_t task;
} DeviceState;

class Device {

protected:
	static Device *instance;

	Device() : greenTask(nullptr), blueTask(nullptr) {
		greenState = new DeviceState();
		blueState = new DeviceState();
		semaphore = xSemaphoreCreateMutex();
		if (semaphore == nullptr) {
			// TODO: handle it.
		}
	}

public:
	SemaphoreHandle_t semaphore;

	DeviceState *greenState;

	DeviceState *blueState;

	Task *greenTask;

	Task *blueTask;

	Device(Device &other) = delete;

	void operator=(const Device &) = delete;

	static Device *getInstance();

	void setup();

	DeviceState *getState() const;

	Task *getTask() const;

	inline uint8_t getBrightness() const {
		return greenState->brightness;
	}

	inline bool isOn() const {
		return greenState->flags.on;
	}

	void swapState();

	uint32_t getSerializationSize() const;

	void serialize(RandomAccess *randomAccess) const;

	void deserialize(RandomAccess *randomAccess);
};

#endif// OZERO_RGB_DEVICE_H
