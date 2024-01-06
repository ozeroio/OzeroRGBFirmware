# Ozero RGB Firmware

App repo: **https://github.com/ozeroio/OzeroRGBApp**

NOTE: Only works on ESP32 devices running FreeRTOS.

## ops

Deploy with:
```
 pio -c clion run --target upload -e nodemcuv2
```

Monitor the device with:
```
 pio device monitor
```

## How to enable support for a new task.

- Create you unique task getCode into ```src/effects/effect.h``` by adding a new entry into EffectCode.

```c++
typedef enum {
    NONE = 0,
    ...,
    MY_AWESOME_EFFECT
} EffectCode;
```

- Create a new class into ```src/task/``` with the following template as an example:

my_awesome_effect.h

```c++
#ifndef OZERO_RGB_MY_AWESOME_EFFECT_H
#define OZERO_RGB_MY_AWESOME_EFFECT_H

#include <task.h>
#include <config.h>

typedef struct {
    uint32_t param0;
    uint32_t param1;
    uint8_t param2;
    int32_t param3;
} MyOwesomeEffectState;

class MyOwesomeEffect : public virtual Effect {

protected:

    static MyOwesomeEffect* instance;

    MyOwesomeEffect();

public:
    
    MyOwesomeEffect(MyOwesomeEffect &other) = delete;

    void operator=(const MyOwesomeEffect &) = delete;

    static MyOwesomeEffect *getInstance();

    void setup() override;

    uint32_t loop() override;

    uint32_t getSerializationSize() override;

    void serialize(RandomAccess *randomAccess) override;

    void deserialize(RandomAccess *randomAccess) override;
};

#endif // OZERO_RGB_MY_AWESOME_EFFECT_H
```

Then, implement it:

my_awesome_effect.cpp
```c++
#include "my_awesome_effect.h"

MyOwesomeEffect *MyOwesomeEffect::instance = nullptr;

MyOwesomeEffect::MyOwesomeEffect() : Task(EffectCode::MY_AWESOME_EFFECT, new MyOwesomeEffectState(), new MyOwesomeEffectState()) {
}

// It's fine that every task is a singleton. But it doesn't need to do!
MyOwesomeEffect *MyOwesomeEffect::getInstance() {

    // Ran once before any task is created.
    if (instance == nullptr) {
        instance = new MyOwesomeEffect();
    }
    return instance;
}

// Called once, once your task is selected to run.
void MyOwesomeEffect::setup() {
    Effect::setup();
    
    // Do any needed setup.
    // At this point your task has all its params deserialized and read to be used.
}

// Called repeatedly. It's recommended that you a > 0 value, since this is the number of cycles
// between calls and FreeRTOS sleeps in cycle intervals. Each cycle is 1ms. So, your can't sleep 
// less than 1 millisecond.
uint32_t MyOwesomeEffect::loop() {

    // Do the awesomeness.
    
    // Return any delay im cycles.
    return 1;
}

// It's required so the communication mechanism knows how bit is your task for serialization purposes.
// BTW, you can exchange more than the params structure with the app. 
uint32_t MyOwesomeEffect::getSerializationSize() {
    return sizeof(MyOwesomeEffectState);
}

// Implement how your task's params are serialized into a RandomAccess.
// NOTE: Should come from greenState.
void MyOwesomeEffect::serialize(RandomAccess *randomAccess) {
    auto state = (MyOwesomeEffectState *) greenState;
    xSemaphoreTake(semaphore, portMAX_DELAY);
    randomAccess->writeUnsignedInt(state->param0);
    randomAccess->writeUnsignedInt(state->param1);
    randomAccess->readUnsignedChar(state->param2);
    randomAccess->writeInt(state->param3);
    xSemaphoreGive(semaphore);
}

// Implement how your task's params are deserialized from a RandomAccess.
// NOTE: Should go into blueState.
void MyOwesomeEffect::deserialize(RandomAccess *randomAccess) {
    auto state = (MyOwesomeEffectState *) blueState;
    xSemaphoreTake(semaphore, portMAX_DELAY);
    state->param0 = randomAccess->readUnsignedInt();
    state->param1 = randomAccess->readUnsignedInt();
    state->param2 = randomAccess->readUnsignedChar();
    state->param3 = randomAccess->readInt();
    xSemaphoreGive(semaphore);
}

```

- Include it into ```main.h```:

```
#include "effects/my_awesome_effect.h"
```

- Enable it into ```main.cpp``` by registering it to the effects for the device:

```
void setup() {
    ...
    
    Task::registerTask(MyOwesomeEffect::getInstance());
```

## Current effects

Breathing formulas:

- Circular Wave
```math
255\cdot\sqrt{1.0-\operatorname{abs}\left(2\cdot\left(\frac{x}{256}\right)-1\right)^{2}}
```

- Square Wave
```math
255\cdot\left(1.0-\operatorname{abs}\left(2\cdot\left(\frac{x}{256}\right)-1\right)\right)
```

- Gaussian Wave
```math
255\cdot e^{-\frac{\left(\frac{\frac{x}{p}-b}{g}\right)^{2}}{2}}
```

https://makersportal.com/blog/2020/3/27/simple-breathing-led-in-arduino#triangle=

## Message protocol

Each message type should be exchanged get the appropriated topic.

The MAX_INGESTIBLE_PAYLOAD_SIZE = 512 is the max length of a message (max eeprom capacity).

### Topics:

- **Discovery**: device/discovery. Host broadcasting to device message. On a device receives this message, it should schedule a **Presence** message.
- **Presence**: device/presence. Device to host messages indicating the device is available.
- **Configure**: device/configure. Host to device messages. The host wants the device to receive a new configuration.
- **Description**: device/description. Device to host messages. The device is reporting its configuration to the host.
- **Describe**: device/describe. Host to device messages. The host wants the device to describe itself - its configuration.
- **Ping**: device/ping. Host to device requesting acknowledgement.
- **Pong**: device/pong. Device to host reporting ping.
- **Req Version**: device/req/version. Host to device requesting its running firmware version.
- **Res Version**: device/res/version. Device to host reporting its running firmware version.

### Discovery message

MESSAGE: `""`

Discovery is broadcast with no payload.

### Presence message

Device to host messages indicating the device is available.

MESSAGE: `"DEVICE_ID:NUM_LEDS:NUM_SUPPORTED_TASKS[:EFFEC_0_CODE:...:TASK_N_CODE]"`

Ex:
- `0:130:2:0:1`, meaning device 0, 130 LEDs, with 2 available effects, 0 and 1.
- `1:130:1:1`, meaning device 1, 130 LEDs with 1 available task, 1.
- `2:130:0`, meaning device 2, 130 LEDs with 0 available effects.

NOTE: Each entry is an `uint32_t`.

### Configure message

Host to device messages. The host wants the device to receive a new configuration.

MESSAGE: `"DEVICE_ID:FLAGS:BRIGHTNESS:TASK_CODE[:EFFEC_PARAM_0:...:EFFEC_PARAM_N]"`

Ex:
- `{0,1,255,2,255,1}`, meaning device 0, on, full (255) brightness, task 2 with 255 and 1 as parameters.
- `{0,0,255,1,1}`, meaning device 0, off, full (255) brightness, task 1 with 1 as parameter.
- `{0,1,0,0}`, meaning device 0, on, brightness 0, task 0 with no state.

NOTE: Each entry is an `uint32_t`, except task parameter that can be a type of any size.

### Description message

Device to host messages. The device is reporting its configuration to the host.

MESSAGE: `"DEVICE_ID:FLAGS:BRIGHTNESS:TASK_CODE[:EFFEC_PARAM_0:...:EFFEC_PARAM_N]"`

Ex:
- `{0,1,255,2,255,1}`, meaning device 0, on, full (255) brightness, task 2 with 255 and 1 as parameters.
- `{0,0,255,1,1}`, meaning device 0, off, full (255) brightness, task 1 with 1 as parameter.
- `{0,1,0,0}`, meaning device 0, on, brightness 0, task 0 with no state.

### Describe message

MESSAGE: `"DEVICE_ID"`

Only contains the DEVICE ID.

### Ping message

MESSAGE: `"DEVICE_ID"`

Only contains the DEVICE ID.

### Pong message

MESSAGE: `"DEVICE_ID"`

Only contains the DEVICE ID.

### Req version

MESSAGE: `"DEVICE_ID"`

Only contains the DEVICE ID.

### Res message

MESSAGE: `"DEVICE_ID:DEVICE_VERSION_MAJOR:DEVICE_VERSION_MINOR:DEVICE_VERSION_PATCH:DEVICE_VERSION_BUILD"`

Contains the DEVICE ID and the SemVer of the firmware running on the device.

NOTE: Each SemVer elements are `uin8_t`.

### Secrets.h

Create the `secrets.h` w/ the following content (replacing with your own data): 

```c++
#ifndef OZERO_RGB_SECRET_H
#define OZERO_RGB_SECRET_H

#define SECRET_WIFI_SSID "<WIFI SSID>"
#define SECRET_WIFI_PASS "<WIFI PASSWORD>"

#define SECRET_MQTT_BROKER_HOST "<MQTT IP>"
#define SECRET_MQTT_BROKER_PORT 3963
#define SECRET_MQTT_BROKER_USER "MQTT USER"
#define SECRET_MQTT_BROKER_PASS "<MQTT PASSWORD>"

#endif// OZERO_RGB_SECRET_H
```