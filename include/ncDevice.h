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
        
        bool registerProtocol(channel_t channel, NCProtocol *protocol);

        bool sendPacket(channel_t channel, uint8_t *data, uint8_t length);
        
        void loop();
};