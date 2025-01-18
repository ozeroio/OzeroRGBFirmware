#include <device.h>
#include <persistence.h>
#include <processor.h>
#include <strip.h>

#define PROCESSING_TASK_FINISH_BIT 0x1

TaskHandle_t Processor::processorTask = nullptr;
SemaphoreHandle_t Processor::finishSemaphore = xSemaphoreCreateBinary();
SemaphoreHandle_t Processor::notifySemaphore = xSemaphoreCreateBinary();

// Runs on the core 1 - processing task.
// Runs every time a task swap happens. Then, keeps executing the current task
// until it is notified to TASK_FINISH. Then finishes task and releases the
// finishSemaphore - to indicate all is done. Then goes for an eternal sleep until gets killed.
void Processor::processingLoop(void *parameters) {
	auto task = static_cast<Task *>(parameters);

	if (task != nullptr) {

		// Holds the notification bits. Some killing task might notify us by setting
		// the PROCESSING_TASK_FINISH_BIT prior to kill us.
		uint32_t notificationValue = 0;

		// In case the task got a task to run. Set it up.
		task->setup();

		// Makes sure we consume any available notification.
		xSemaphoreTake(notifySemaphore, 0);

		// Then calls its loop until it gets notified.
		for (;;) {

			// Check for notification, clear task-kill-bit after saving it into notificationValue.
			if (xTaskNotifyWait(0, PROCESSING_TASK_FINISH_BIT, &notificationValue, 0) == pdTRUE) {

				// The PROCESSING_TASK_FINISH_BIT was set. Finish the task.
				if (notificationValue & PROCESSING_TASK_FINISH_BIT) {
					finishTaskWaitTermination(task);
				}
			}

			auto delay = task->loop();

			// Works like vTaskDelay, but allows the other task to wait it up in case it wants to finish
			// this processing task.
			xSemaphoreTake(notifySemaphore, delay / portTICK_PERIOD_MS);
		}
	}
}

// Runs on the core 0 - communication task.
// Runs every time a task swap happens. Kills current running task and sets up a new one.
// This method run in the core 0, and the processing loop runs on core 1.
void Processor::notify() {

	// Make sure current task is properly terminated.
	stopCurrentTask();

	// Device has been configured for a new state (blue state populated).
	// Swap it to make it green (current).
	auto device = Device::getInstance();
	device->swapState();

	Persistence::getInstance()->persistDevice();

	// Must be moved to another place.
	auto strip = Strip::getInstance();
	strip->clearStrip();
	Strip::setBrightness(device->getBrightness());
	Strip::showStrip();

	if (device->isOn()) {

		auto task = device->getTask();
		if (task != nullptr) {

			// Create a new task(RTOS) to run this device's task.
			// processingLoop will receive the task as parameter and
			// call its loop method repetitively.
			xTaskCreatePinnedToCore(
					Processor::processingLoop,
					PROC_TASK_NAME,
					PROC_TASK_STACK_SIZE,
					task,
					PROC_TASK_PRIORITY,
					&processorTask,
					PROC_TASK_CODE_ID);
		}
	}
}

// Runs on the core 0.
void Processor::stopCurrentTask() {

	if (processorTask == nullptr) {
		return;
	}

	// Notify processing task that it will be terminated.
	// Processing task is running on core 1 and this method is running on core 0 (with main or communication tasks).
	xTaskNotify(processorTask, PROCESSING_TASK_FINISH_BIT, eSetBits);

	// By giving this finishSemaphore, this task notifies the processing task to stop its
	// delay and resume in case it's sleeping.
	xSemaphoreGive(notifySemaphore);

	// Wait for the processing task to finishing up and release the finishSemaphore it is holding.
	xSemaphoreTake(finishSemaphore, (TickType_t) PROCESSOR_MAX_TICKS_KILL_TASK_WAIT);

	// Then, delete the processing task.
	vTaskDelete(processorTask);

	processorTask = nullptr;
}

// Runs on the core 1.
void Processor::finishTaskWaitTermination(Task *task) {

	// Give the task the chance to finish up its things.
	task->finish();

	// Release the finishSemaphore signalling that we are done finishing this task.
	// This allowed the killing task to proceed since it's waiting on us to release this finishSemaphore.
	xSemaphoreGive(finishSemaphore);

	// After releasing finishSemaphore, wait for the comm task to kill this processing task.
	for (;;) {
		taskYIELD();
	}
}
