#include "sdoProtocol.h"

SDOHost::SDOHost(NCDevice* device, channel_t channel) : NCProtocol(device, channel) {
    _data = NULL;
    _dataSize = 0;
    _state = SDOHost_t::IDLE;
    _transmitted = 0;
}

SDOHost::~SDOHost() {
}

bool SDOHost::setData(uint8_t* data, uint8_t size) {
    if (_state != SDOHost_t::IDLE) return false;

    _data = data;
    _dataSize = size;
    return true;
}

bool SDOHost::sendData() {
    if (_state != SDOHost_t::IDLE) return false;

    uint8_t data[2] = {
            SDO_DATA_START,
            _dataSize
    };

    _state = SDOHost_t::SEND_DATA;
    _transmitted = 0;

    return sendPacket(data, 2);
}

void SDOHost::addCallback(void(*callback)(uint8_t*, uint8_t)) {
    _callbacks.push_back(callback);
}

void SDOHost::onRecieve(uint8_t* data, uint8_t size) {
    if (size < 1) return;

    uint8_t command = data[0];

    //Serial.println("SDO Host recieved command: " + String(command));

    if (command == SDO_REQUEST_DATA && _state == SDOHost_t::IDLE) {
        sendData();
    }
    else if (command == SDO_DATA_START && _state == SDOHost_t::IDLE) {
        // reset recieving buffer
        _state = SDOHost_t::RECIEVING;
        _bufferSize = data[1];
        _buffer = new uint8_t[_bufferSize];
        _currentPos = 0;
    }
    else if (command == SDO_DATA_PACKET && _state == SDOHost_t::RECIEVING) {
        // write data to buffer
        for (uint8_t i = 1; i < size; i++) {
            _buffer[_currentPos] = data[i];
            _currentPos++;
        }
    }
    else if (command == SDO_DATA_END && _state == SDOHost_t::RECIEVING) {
        _state = SDOHost_t::IDLE;

        // call callbacks
        for (uint8_t i = 0; i < _callbacks.size(); i++) {
            _callbacks[i](_buffer, _bufferSize);
        }

        delete[] _buffer;
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