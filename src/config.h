#ifndef OZERO_RGB_CONFIG_H
#define OZERO_RGB_CONFIG_H

#include <FastLED.h>
#include <secrets.h>

#define LED_PIN (23)
#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#define NUM_LEDS_OFFICE_WINDOW (88)// Office windows
#define NUM_LEDS_SUITE_WINDOW (88) // Suite windows
#define NUM_LEDS_LUCAS_WINDOW (60) // Lucas window
#define NUM_LEDS_HOUSE_FRONT (371) // Casa
#define NUM_LEDS_LUCAS_DESK (54)   // Lucas desk
#define NUM_LEDS_DAVI_DESK (101)   // Davi desk
#define NUM_LEDS_OFFICE_DESK (97)  // Office desk
#define NUM_LEDS_KITCHEN_TOP (84)  // Kitchen
#define NUM_LEDS_DAVI_WINDOW (60)  // Davi window
#define NUM_LEDS_DAVI_CLOSET (122) // Davi closet
#define NUM_LEDS_LIVING_ROOM (172) // Living room big window
#define NUM_LEDS_LUCAS_PC (22)	   // Living room big window
#define NUM_LEDS_TEST (150)		   // Default strip

#define NUM_LEDS NUM_LEDS_TEST
#define HALF_NUM_LEDS ((NUM_LEDS) / 2)

#define DEVICE_OFFICE_WINDOW (0)// Office windows
#define DEVICE_SUITE_WINDOW (1) // Suite windows
#define DEVICE_LUCAS_WINDOW (2) // Lucas window
#define DEVICE_HOUSE_FRONT (3)	// House front
#define DEVICE_LUCAS_DESK (4)	// Lucas desk
#define DEVICE_DAVI_DESK (5)	// Davi desk
#define DEVICE_OFFICE_DESK (6)	// Office desk
#define DEVICE_KITCHEN_TOP (7)	// Kitchen
#define DEVICE_DAVI_WINDOW (8)	// Davi window
#define DEVICE_DAVI_CLOSET (9)	// Davi closet
#define DEVICE_LIVING_ROOM (10) // Living room big window
#define DEVICE_LUCAS_PC (11)	// Living room big window
#define DEVICE_TEST (255)		// Default strip

#define DEVICE_ID DEVICE_TEST

#define DEVICE_VERSION_MAJOR (5)
#define DEVICE_VERSION_MINOR (1)
#define DEVICE_VERSION_PATCH (0)
#define DEVICE_VERSION_BUILD (0)
#define DEVICE_MAC_ADDRESS_SIZE (12)
#define DEVICE_MAC_ADDRESS_PATTERN ("%012X")

#define EEPROM_SIZE (512)
#define MAX_INGESTIBLE_PAYLOAD_SIZE (512)

#define PROC_TASK_NAME ("Processing")
#define PROC_TASK_STACK_SIZE (4096)
#define PROC_TASK_CODE_ID (1)
#define PROC_TASK_PRIORITY (1)

#define COMM_TASK_NAME ("Communication")
#define COMM_TASK_STACK_SIZE (2048)
#define COMM_TASK_CODE_ID (0)
#define COMM_TASK_PRIORITY (0)

#define COMM_TOPIC_DISCOVERY ("device/discovery")
#define COMM_TOPIC_PRESENCE ("device/presence")
#define COMM_TOPIC_CONFIGURE ("device/configure")
#define COMM_TOPIC_DESCRIPTION ("device/description")
#define COMM_TOPIC_DESCRIBE ("device/describe")
#define COMM_TOPIC_PING ("device/ping")
#define COMM_TOPIC_PONG ("device/pong")
#define COMM_TOPIC_REQ_VERSION ("device/req/version")
#define COMM_TOPIC_RES_VERSION ("device/res/version")

#define COMM_WIFI_SSID SECRET_WIFI_SSID
#define COMM_WIFI_PASS SECRET_WIFI_PASS

#define COMM_MQTT_BROKER_HOST SECRET_MQTT_BROKER_HOST
#define COMM_MQTT_BROKER_PORT SECRET_MQTT_BROKER_PORT
#define COMM_MQTT_BROKER_USER SECRET_MQTT_BROKER_USER
#define COMM_MQTT_BROKER_PASS SECRET_MQTT_BROKER_PASS

#endif// OZERO_RGB_CONFIG_H
