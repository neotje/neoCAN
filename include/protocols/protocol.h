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

        /**
         * @brief send packet on the channel this protocol is registered.
         * 
         * @param data 
         * @param length 
         * @return true 
         * @return false 
         */
        bool sendPacket(uint8_t *data, uint8_t length);

        /**
         * @brief Called when there is a packet available on this channel.
         * 
         * @param data Data object
         * @param size Data size
         */
        virtual void onRecieve(uint8_t *data, uint8_t size) = 0;

        /**
         * @brief Basically loop function.
         */
        virtual void onUpdate() = 0;
};