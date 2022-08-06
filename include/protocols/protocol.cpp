#include "protocol.h"
#include "ncDevice.h"

NCProtocol::NCProtocol(NCDevice *device, uint16_t channel) {
    _device = device;
    _channel = channel;

    _device->registerProtocol(_channel, this);
}

bool NCProtocol::sendPacket(uint8_t *data, uint8_t length) {
    return _device->sendPacket(_channel, data, length);
}