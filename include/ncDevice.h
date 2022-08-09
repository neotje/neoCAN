#pragma once

#include <Arduino.h>
#include <CAN.h>
#include <map>

#include "protocols/protocol.h"
#include "globals.h"

class NCDevice {
    private:
        nodeId_t _nodeId;
        CANControllerClass *_can;
        std::map<channel_t, NCProtocol*> *_protocols;

    public:
        NCDevice(uint16_t nodeId, CANControllerClass *can);
        ~NCDevice();
        
        nodeId_t getNodeId() { return _nodeId; }
        
        /**
         * @brief Register a protocol on a channel. Channel 0 is reserved
         * 
         * @param channel channel number
         * @param protocol protocol object
         * @return true 
         * @return false if protocol is already registered
         */
        bool registerProtocol(channel_t channel, NCProtocol *protocol);

        /**
         * @brief Send packet on a specific channel. maximum data length is 8
         * 
         * @param channel Channel number
         * @param data Data object
         * @param length Data length
         * @return true 
         * @return false
         */
        bool sendPacket(channel_t channel, uint8_t *data, uint8_t length);
        
        void loop();
};