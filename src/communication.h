#ifndef OZERO_RGB_COMMUNICATION_H
#define OZERO_RGB_COMMUNICATION_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <config.h>
#include <device.h>
#include <freertos/queue.h>
#include <stdint-gcc.h>

typedef enum {
	NO_OPERATION = (0x01 << 0),
	REPORT_PRESENCE_OPERATION = (0x01 << 1),
	REPORT_DESCRIPTION_OPERATION = (0x01 << 2),
	REPORT_PONG_OPERATION = (0x01 << 3),
	NOTIFY_DEVICE_UPDATED = (0x01 << 4),
	REQUEST_VERSION_OPERATION = (0x01 << 5)
} Operation;

class Communication {

	WiFiClient *wifi;
	PubSubClient *mqtt;

	char deviceMacAddress[DEVICE_MAC_ADDRESS_SIZE];

protected:
	static Communication *instance;

	static Operation scheduledOperation;

	Communication() : deviceMacAddress("") {
		wifi = new WiFiClient();
		mqtt = new PubSubClient(*wifi);
	};

public:
	TaskHandle_t communicationTask{};

	Communication(Communication &other) = delete;

	void operator=(const Communication &) = delete;

	static Communication *getInstance();

	static void taskHandler(void *parameters);

	static void callback(const char *topic, uint8_t *payload, uint32_t length);

	void setup();

	void sendPresenceMessage();

	void sendDescriptionMessage();

	void sendPongMessage();

	void sendVersionMessage();

	void communicationLoop();
};

#endif// OZERO_RGB_COMMUNICATION_H
