#include "ncDevice.h"

NCDevice::NCDevice(uint16_t nodeId, CANControllerClass* can) {
    _nodeId = nodeId;
    _can = can;
    _protocols = new std::map<uint16_t, NCProtocol*>();
}

NCDevice::~NCDevice() {
    delete _protocols;
}

bool NCDevice::registerProtocol(uint16_t channel, NCProtocol* protocol) {
    if (channel < 1) {
        return false;
    }
    if (_protocols->find(channel) != _protocols->end()) {
        return false;
    }
    _protocols->insert(std::pair<uint16_t, NCProtocol*>(channel, protocol));
    return true;
}

bool NCDevice::sendPacket(uint16_t channel, uint8_t* data, uint8_t length) {
    if (channel < 1) {
        return false;
    }

    if (!_can->beginExtendedPacket(toCanId(_nodeId, channel))) return false;
    _can->write(data, length);
    return _can->endPacket();
}

void NCDevice::loop() {
    int size = _can->parsePacket();

    // packet has content, is extended, and is for this device
    if (size > 0 && _can->packetExtended() && getNodeIdFromCanId(_can->packetId()) == _nodeId) {
        uint16_t channel = getChannelFromCanId(_can->packetId());

        // check if channel is registered and call onRecieve()
        if (_protocols->find(channel) != _protocols->end()) {
            uint8_t* data = new uint8_t[size];
            _can->readBytes(data, size);

            _protocols->at(channel)->onRecieve(data, size);

            delete[] data;
        }
    }

    // loop through all protocols and call onUpdate()
    for (std::map<uint16_t, NCProtocol*>::iterator it = _protocols->begin(); it != _protocols->end(); ++it) {
        it->second->onUpdate();
    }
}