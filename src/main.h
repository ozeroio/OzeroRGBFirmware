#define __OZEROIO_IO_INTERNAL_EEPROM_SUPPORT_ENABLED__ 1

#include <EEPROM.h>
#include <FastLED.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <freertos/FreeRTOS.h>

#include <RandomAccessInternalEeprom/RandomAccessInternalEeprom.h>

#include <config.h>
#include <utils.h>

#include <communication.h>
#include <device.h>
#include <persistence.h>
#include <processor.h>
#include <strip.h>

#include <effects/breathing.h>
#include <effects/chase.h>
#include <effects/color.h>
#include <effects/fire.h>
#include <effects/shift.h>
#include <effects/sparkle.h>
#include <effects/wave.h>
