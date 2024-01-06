#ifndef OZERO_RGB_EFFECT_H
#define OZERO_RGB_EFFECT_H

#include <task.h>

typedef enum {
	NONE = 0,
	COLOR_EFFECT,
	FIRE_EFFECT,
	WAVE_EFFECT,
	CHASE_EFFECT,
	SPARKLE_EFFECT,
	BREATHING_EFFECT,
	SHIFT_EFFECT
} EffectCode;

class Effect : public virtual Task {
};

#endif// OZERO_RGB_EFFECT_H
