#ifndef OZERO_RGB_DEVICE_CONFIG_H
#define OZERO_RGB_DEVICE_CONFIG_H

#include <RandomAccess/RandomAccess.h>
#include <device.h>

/**
 * Persistence is a singleton class ta interfaces some persisting
 * memory in order to save the devices state.
 */
class Persistence {

	// Access to some physical storage.
	// Generally some EEPROM RandomAccess instance.
	RandomAccess *randomAccess;

protected:
	static Persistence *instance;

	Persistence() : randomAccess(nullptr) {
	}

public:
	Persistence(Persistence &other) = delete;

	void operator=(const Persistence &) = delete;

	static Persistence *getInstance();

	void setup(RandomAccess *);

	RandomAccess *getRandomAccess(boolean = true);

	/**
	 * This class knows the Device class.
	 * It calls it's serialize method passing this RandomAccess
	 * so that the device gets saved into the persisting memory.
	 */
	void persistDevice();
};

#endif// OZERO_RGB_DEVICE_CONFIG_H
