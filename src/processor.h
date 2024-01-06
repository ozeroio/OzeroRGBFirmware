#ifndef OZERO_RGB_PROCESSOR_H
#define OZERO_RGB_PROCESSOR_H

#define PROCESSOR_MAX_TICKS_KILL_TASK_WAIT 5000

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <task.h>

/**
 * Processor class provides the encapsulation for a task.
 * It run pinned to a core, and loops the processingLoop method indefinitely.
 */
class Processor {

	Processor() = default;

	/**
	 * Processing task handler.
	 */
	static TaskHandle_t processorTask;

	/**
	 * Semaphore to coordinate communication with the task running on a different core.
	 */
	static SemaphoreHandle_t finishSemaphore;

	/**
	 * Semaphore to allow the processing task to delay between interaction waiting on this
	 * finishSemaphore. This is necessary to avoid using vTaskDelay. When vTaskDelay the task has
	 * to wait until the time runs out.
	 */
	static SemaphoreHandle_t notifySemaphore;

	/**
	 * Sends signal to the task to finish, then kills it.
	 */
	static void stopCurrentTask();

	/**
	 * Finishes the current task and wait forever to be terminated.
	 */
	static void finishTaskWaitTermination(Task *task);

public:

	/**
	 * Loops the current tasks.
	 */
	static void processingLoop(void *);

	/**
	 * Terminates the current task then runs new task from the device.
	 */
	static void notify();
};


#endif// OZERO_RGB_PROCESSOR_H
