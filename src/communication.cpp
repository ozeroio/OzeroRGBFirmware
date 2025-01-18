
#include <RandomAccessByteArray/RandomAccessByteArray.h>
#include <communication.h>
#include <config.h>
#include <map>
#include <persistence.h>
#include <processor.h>
#include <vector>

Communication *Communication::instance = nullptr;
Operation Communication::scheduledOperation = NO_OPERATION;

Communication *Communication::getInstance() {

	// Ran once before any task is created.
	if (instance == nullptr) {
		instance = new Communication();
	}
	return instance;
}

// Main task.
void Communication::setup() {
	WiFiClass::mode(WIFI_STA);
	WiFi.begin(COMM_WIFI_SSID, COMM_WIFI_PASS);

	// Setup MAC address based on DEVICE_ID.
	sprintf(deviceMacAddress, DEVICE_MAC_ADDRESS_PATTERN, DEVICE_ID);

	log_i("WiFi.connecting to: %s", COMM_WIFI_SSID);

	while (WiFiClass::status() != WL_CONNECTED) {
		vTaskDelay(500 / portTICK_PERIOD_MS);
		log_i("WiFi.status: %s. Retrying in 0.5 second.", WiFiClass::status());
	}

	log_i("WiFi connected. IP address: %s", WiFi.localIP());

	mqtt->setServer(COMM_MQTT_BROKER_HOST, COMM_MQTT_BROKER_PORT);
	mqtt->setCallback(callback);

	while (!mqtt->connected()) {
		log_i("MQTT client id: %s. Connecting to mqtt broker...", deviceMacAddress);
		if (mqtt->connect(deviceMacAddress, COMM_MQTT_BROKER_USER, COMM_MQTT_BROKER_PASS)) {
			log_i("Public mqtt broker connected.");
		} else {
			log_i("Connection failed with state: %s. Retrying in 1 second.", mqtt->state());
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
	}

	mqtt->subscribe(COMM_TOPIC_DISCOVERY);
	mqtt->subscribe(COMM_TOPIC_REQ_VERSION);
	mqtt->subscribe(COMM_TOPIC_DESCRIBE);
	mqtt->subscribe(COMM_TOPIC_CONFIGURE);
	mqtt->subscribe(COMM_TOPIC_PING);

	sendPresenceMessage();

	xTaskCreatePinnedToCore(
			reinterpret_cast<TaskFunction_t>(Communication::taskHandler),
			COMM_TASK_NAME,
			COMM_TASK_STACK_SIZE,
			this,
			COMM_TASK_PRIORITY,
			&communicationTask,
			COMM_TASK_CODE_ID);
}

void Communication::sendPresenceMessage() {
	std::map<uint32_t, Task *> *m = Task::getRegisteredTasks();

	// 1 int for device id
	// 1 int for number of LEDs
	// 1 int for the size of effects
	// N for each effect
	uint32_t len = (3 + m->size()) * sizeof(unsigned int);
	uint8_t presenceMessage[len];
	auto randomAccess = RandomAccessByteArray(presenceMessage, len);
	randomAccess.writeUnsignedInt(DEVICE_ID);
	randomAccess.writeUnsignedInt(NUM_LEDS);
	randomAccess.writeUnsignedInt(m->size());
	for (auto &it: *m) {
		randomAccess.writeUnsignedInt(it.first);
	}
	mqtt->publish(COMM_TOPIC_PRESENCE, presenceMessage, len);
}

void Communication::sendDescriptionMessage() {
	auto device = Device::getInstance();

	// 4 byte for device id
	// N for the state of device + effect (if any)
	uint32_t size = sizeof(uint32_t) + device->getSerializationSize();
	uint8_t descriptionMessage[size];
	auto randomAccess = RandomAccessByteArray(descriptionMessage, size);
	randomAccess.writeUnsignedInt(DEVICE_ID);
	device->serialize(&randomAccess);

	mqtt->publish(COMM_TOPIC_DESCRIPTION, descriptionMessage, size);
}

void Communication::sendPongMessage() {

	// 4 byte for device id
	uint32_t len = sizeof(uint32_t);
	uint8_t pongMessage[len];
	auto randomAccess = RandomAccessByteArray(pongMessage, len);
	randomAccess.writeUnsignedInt(DEVICE_ID);

	mqtt->publish(COMM_TOPIC_PONG, pongMessage, len);
}

void Communication::sendVersionMessage() {

	// 4 byte for device id
	// 4 byte for version
	uint32_t len = 2 * sizeof(uint32_t);
	uint8_t versionMessage[len];
	auto randomAccess = RandomAccessByteArray(versionMessage, len);
	randomAccess.writeUnsignedInt(DEVICE_ID);
	randomAccess.writeUnsignedChar(DEVICE_VERSION_MAJOR);
	randomAccess.writeUnsignedChar(DEVICE_VERSION_MINOR);
	randomAccess.writeUnsignedChar(DEVICE_VERSION_PATCH);
	randomAccess.writeUnsignedChar(DEVICE_VERSION_BUILD);

	mqtt->publish(COMM_TOPIC_RES_VERSION, versionMessage, len);
}

void Communication::callback(const char *topic, uint8_t *payload, uint32_t length) {

	if (length > MAX_INGESTIBLE_PAYLOAD_SIZE) {
		return;
	}

	if (strcmp(topic, COMM_TOPIC_DISCOVERY) == 0) {
		scheduledOperation = REPORT_PRESENCE_OPERATION;
		return;
	}

	if (strcmp(topic, COMM_TOPIC_PING) == 0) {
		scheduledOperation = REPORT_PONG_OPERATION;
		return;
	}

	if (strcmp(topic, COMM_TOPIC_REQ_VERSION) == 0) {
		scheduledOperation = REQUEST_VERSION_OPERATION;
		return;
	}

	auto randomAccess = RandomAccessByteArray(payload, length);

	// Always skip 1 int (4 bytes).
	auto deviceId = randomAccess.readUnsignedInt();

	if (deviceId != DEVICE_ID) {

		// Message not designated to this device.
		return;
	}

	if (strcmp(topic, COMM_TOPIC_DESCRIBE) == 0) {
		scheduledOperation = REPORT_DESCRIPTION_OPERATION;
		return;
	}

	if (strcmp(topic, COMM_TOPIC_CONFIGURE) == 0) {
		if (length > 1) {

			auto device = Device::getInstance();

			// Random access is already skipped by 1 int (4 bytes).
			device->deserialize(&randomAccess);
			scheduledOperation = NOTIFY_DEVICE_UPDATED;
		}
		return;
	}
}

/**
 * Communication loop with mqtt server.
 */
void Communication::communicationLoop() {
	mqtt->loop();
	if (scheduledOperation & REPORT_DESCRIPTION_OPERATION) {
		sendDescriptionMessage();
	}
	if (scheduledOperation & REPORT_PONG_OPERATION) {
		sendPongMessage();
	}
	if (scheduledOperation & REQUEST_VERSION_OPERATION) {
		sendVersionMessage();
	}
	if (scheduledOperation & REPORT_PRESENCE_OPERATION) {
		sendPresenceMessage();
	}
	if (scheduledOperation & NOTIFY_DEVICE_UPDATED) {
		Processor::notify();
	}
	scheduledOperation = NO_OPERATION;
}

/**
 * Communication handler with mqtt server.
 *
 * @param parameters The communication class instance.
 */
void Communication::taskHandler(void *parameters) {
	auto comm = static_cast<Communication *>(parameters);

	for (;;) {
		comm->communicationLoop();
		vTaskDelay(portTICK_PERIOD_MS);
	}
}
