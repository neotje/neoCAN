#pragma once

#include <Arduino.h>
#include <CAN.h>
#include <neoCAN.h>

NCDevice *device;
SDOHost *host;
SDOHost *host2;

void sdoCallback(uint8_t* data, uint8_t size) {
    Serial.print("sdoCallback: ");
    Serial.print((char*)data);
}

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        ; // wait for serial port to connect
    }

    Serial.println("CAN State Device...");

    pinMode(PIN_CAN_STANDBY, OUTPUT);
    digitalWrite(PIN_CAN_STANDBY, LOW);

    pinMode(PIN_CAN_BOOSTEN, OUTPUT);
    digitalWrite(PIN_CAN_BOOSTEN, HIGH);

    if(!CAN.begin(250000)) {
        Serial.println("CAN initialization failed");
        while(1);
    }

    device = new NCDevice(1, &CAN);
    host = new SDOHost(device, 1);
    host2 = new SDOHost(device, 2);

    host->addCallback(sdoCallback);
    host2->addCallback(sdoCallback);

    char* data = "Hello World!\n";
    host->setData((uint8_t *)data, strlen(data) + 1);

    data = "Hello Universe!\n";
    host2->setData((uint8_t *)data, strlen(data) + 1);
}

void loop() {
    device->loop();
}