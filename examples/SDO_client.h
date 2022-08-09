#pragma once

#include <Arduino.h>
#include <CAN.h>
#include <neoCAN.h>

NCDevice* device;
SDOClient* client;
SDOClient* client2;

uint32_t lastUpdate = -1;

unsigned long start;

void sdoCallback(uint8_t* data, uint8_t size) {
    Serial.print("sdoCallback: ");
    Serial.print((char*)data);
}

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        ; // wait for serial port to connect
    }

    Serial.println("CAN State Manager...");

    pinMode(PIN_CAN_STANDBY, OUTPUT);
    digitalWrite(PIN_CAN_STANDBY, LOW);

    pinMode(PIN_CAN_BOOSTEN, OUTPUT);
    digitalWrite(PIN_CAN_BOOSTEN, HIGH);

    if (!CAN.begin(250000)) {
        Serial.println("CAN initialization failed");
        while (1);
    }

    device = new NCDevice(1, &CAN);
    client = new SDOClient(device, 1);
    client2 = new SDOClient(device, 2);

    client->addCallback(sdoCallback);
    client2->addCallback(sdoCallback);
}

char* data = "Hello Device!\n";

void loop() {
    device->loop();

    if (millis() - lastUpdate > 3000) {
        lastUpdate = millis();

        Serial.println("send request");

        client->sendData((uint8_t *)data, strlen(data) + 1);

        client->sendDataRequest();
        client2->sendDataRequest();
    }
}