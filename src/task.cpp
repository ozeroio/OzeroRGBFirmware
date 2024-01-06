#include "task.h"

std::map<uint32_t, Task *> Task::registeredTasks = {};

Task::Task(const uint32_t code,
		   const void *greenState,
		   const void *blueState) : code(code),
									greenState(const_cast<void *>(greenState)),
									blueState(const_cast<void *>(blueState)) {
	semaphore = xSemaphoreCreateMutex();
	if (semaphore == nullptr) {
		// TODO: handle it.
	}
}

void Task::registerTask(const Task *task) {
	Task::registeredTasks[task->getCode()] = (Task *) task;
}

Task *Task::fetchRegisteredEffect(uint32_t code) {
	auto found = Task::registeredTasks.find(code);
	if (found == Task::registeredTasks.end()) {
		return nullptr;
	}
	return found->second;
}

std::map<uint32_t, Task *> *Task::getRegisteredTasks() {
	return &registeredTasks;
}

uint32_t Task::getCode() const {
	return code;
}

void Task::swapState() {
	xSemaphoreTake(semaphore, portMAX_DELAY);
	auto state = greenState;
	greenState = blueState;
	blueState = state;
	xSemaphoreGive(semaphore);
}

void Task::setup() {
}

void Task::finish() {
}
