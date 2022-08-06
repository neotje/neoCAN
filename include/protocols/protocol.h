#pragma once

#include <Arduino.h>
#include <CAN.h>

#include "globals.h"

class NCDevice;

class NCProtocol {
    private:
        NCDevice *_device;
        channel_t _channel;
    public:
        NCProtocol(NCDevice *device, channel_t channel);
        ~NCProtocol();

        channel_t getChannel() { return _channel; }

        bool sendPacket(uint8_t *data, uint8_t length);

        virtual void onRecieve(uint8_t *data, uint8_t size) = 0;
        virtual void onUpdate() = 0;
};