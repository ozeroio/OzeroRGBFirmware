#ifndef OZERO_TASK_H
#define OZERO_TASK_H

#include <RandomAccess/RandomAccess.h>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <map>

class Task {

	static std::map<uint32_t, Task *> registeredTasks;

protected:
	uint32_t code;

	// Pointer to hold the green state.
	void *greenState;


	// Pointer to hold the blue state.
	void *blueState;

public:
	SemaphoreHandle_t semaphore;

	Task() = default;

	Task(uint32_t code, const void *greenState, const void *blueState);

	static void registerTask(Task *task);

	static Task *fetchRegisteredEffect(uint32_t code);

	static std::map<uint32_t, Task *> *getRegisteredTasks();

	virtual uint32_t getCode() const;

	virtual void swapState();

	virtual void setup();

	/**
	 * Main effect loop function.
	 *
	 * @return num of millis to be waited before calling this loop again.
	 */
	virtual uint32_t loop() = 0;

	virtual void finish();

	virtual uint32_t getSerializationSize() = 0;

	virtual void serialize(RandomAccess *randomAccess) = 0;

	virtual void deserialize(RandomAccess *randomAccess) = 0;
};

#endif// OZERO_TASK_H
