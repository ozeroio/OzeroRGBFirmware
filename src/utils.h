#ifndef OZERO_RGB_UTILS_H
#define OZERO_RGB_UTILS_H

#include <config.h>

class Utils {

protected:
	Utils() = default;

public:
	static void ensureLimits();

	static void shiftRight(uint32_t n = 1);

	static void shiftLeft(uint32_t n = 1);
};

#endif// OZERO_RGB_UTILS_H
