#include "main.h"

void setup() {
	Serial.begin(115200);
	EEPROM.begin(EEPROM_SIZE);

	// Housekeeping routines.
	Utils::ensureLimits();

	// Register all supported effects.
	Task::registerTask(ColorEffect::getInstance());
	Task::registerTask(FireEffect::getInstance());
	Task::registerTask(WaveEffect::getInstance());
	Task::registerTask(ChaseEffect::getInstance());
	Task::registerTask(SparkleEffect::getInstance());
	Task::registerTask(BreathingEffect::getInstance());
	Task::registerTask(ShiftEffect::getInstance());

	// Initialization.
	auto randomAccess = new RandomAccessInternalEeprom(0, EEPROM_SIZE);
	auto strip = Strip::getInstance();
	auto device = Device::getInstance();
	auto persistent = Persistence::getInstance();
	auto communication = Communication::getInstance();

	// Setup.
	strip->setup();
	persistent->setup(randomAccess);
	device->setup();
	communication->setup();

	// Start effect task if any.
	Processor::notify();
}

void loop() {
	log_i(xPortGetFreeHeapSize());
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}
