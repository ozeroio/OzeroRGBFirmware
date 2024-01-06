#include "persistence.h"

Persistence *Persistence::instance = nullptr;

Persistence *Persistence::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new Persistence();
	}
	return instance;
}

void Persistence::setup(RandomAccess *_randomAccess) {
	this->randomAccess = _randomAccess;
}

RandomAccess *Persistence::getRandomAccess(boolean reset) {
	if (reset) {
		randomAccess->seek(0);
	}
	return randomAccess;
}

void Persistence::persistDevice() {
	Device::getInstance()->serialize(getRandomAccess());
}
