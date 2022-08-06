#include "sdoProtocol.h"

SDOHost::SDOHost(NCDevice *device, channel_t channel) : NCProtocol(device, channel) {
    _data = NULL;
    _dataSize = 0;
    _state = SDOHost_t::IDLE;
    _transmitted = 0;
}

SDOHost::~SDOHost() {
}

void SDOHost::onRecieve(uint8_t *data, uint8_t size) {
    if (size < 1) return;

    uint8_t command = data[0];

    Serial.println("SDO Host recieved command: " + String(command));

    if (command == SDO_REQUEST_DATA && _state == SDOHost_t::IDLE) {
        uint8_t data[2] = {
            SDO_DATA_START,
            _dataSize
        };

        sendPacket(data, 2);
        _state = SDOHost_t::SEND_DATA;
        _transmitted = 0;
    }
}

void SDOHost::onUpdate() {
    if (_state == SDOHost_t::SEND_DATA) {
        if (_transmitted < _dataSize) {
            uint8_t start = _transmitted;
            uint8_t end = min(_transmitted + SDO_MAX_PACKET_SIZE, _dataSize);

            uint8_t packet[end - start + 1];
            packet[0] = SDO_DATA_PACKET;

            for (uint8_t i = 0; i < end - start; i++) {
                packet[i + 1] = _data[start + i];
            }

            sendPacket(packet, end - start + 1);
            _transmitted = end;
        }

        if (_transmitted >= _dataSize) {
            uint8_t data[1] = {
                SDO_DATA_END
            };

            sendPacket(data, 1);
            _state = SDOHost_t::IDLE;
        }
    }
}