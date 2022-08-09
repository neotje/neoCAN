#include "sdoProtocol.h"

SDOClient::SDOClient(NCDevice* device, channel_t channel) : NCProtocol(device, channel) {
    _state = SDOClient_t::IDLE;
    _recievedData = NULL;
    _dataSize = 0;
    _currentPos = 0;
}

SDOClient::~SDOClient() {
}

bool SDOClient::sendDataRequest() {
    if (_state == SDOClient_t::IDLE && sendPacket((uint8_t*)&SDO_REQUEST_DATA, 1)) {
        _state = SDOClient_t::WAITING;
        return true;
    }
    return false;
}

bool SDOClient::sendData(uint8_t* data, uint8_t size) {
    if (_state != SDOClient_t::IDLE) return false;

    // send start packet
    uint8_t startPacket[2] = {
        SDO_DATA_START,
        size
    };
    if (!sendPacket(startPacket, 2)) return false;

    uint8_t* buffer;
    uint8_t transmitted = 0;

    while (transmitted < size) {
        uint8_t start = transmitted;
        uint8_t end = min(transmitted + SDO_MAX_PACKET_SIZE, size);
        
        buffer = new uint8_t[end - start + 1];
        buffer[0] = SDO_DATA_PACKET;

        for (uint8_t i = 0; i < end - start; i++) {
            buffer[i + 1] = data[start + i];
        }

        if (!sendPacket(buffer, end - start + 1)) {
            delete[] buffer;
            return false;
        }

        transmitted = end;
        delete[] buffer;
    }

    // send end packet
    uint8_t endPacket[1] = {
        SDO_DATA_END
    };
    if (!sendPacket(endPacket, 1)) return false;

    return true;
}

void SDOClient::addCallback(void(*callback)(uint8_t*, uint8_t)) {
    _callbacks.push_back(callback);
}

void SDOClient::onRecieve(uint8_t* data, uint8_t size) {
    if (size < 1) return;

    uint8_t command = data[0];

    if (command == SDO_DATA_START && _state == SDOClient_t::WAITING) {
        _dataSize = data[1];
        _currentPos = 0;
        _state = SDOClient_t::RECIEVING;

        // allocate memory for the data
        delete[] _recievedData;
        _recievedData = new uint8_t[_dataSize];
        //memset(_recievedData, 0, _dataSize);
    }
    else if (command == SDO_DATA_PACKET && _state == SDOClient_t::RECIEVING) {
        // copy the data to the recievedData
        for (size_t i = 1; i < size; i++) {
            _recievedData[_currentPos] = data[i];
            _currentPos++;
        }
    }
    else if (command == SDO_DATA_END && _state == SDOClient_t::RECIEVING) {
        _state = SDOClient_t::IDLE;
        // call the callbacks
        for (size_t i = 0; i < _callbacks.size(); i++) {
            _callbacks[i](_recievedData, _dataSize);
        }
    }
}

void SDOClient::onUpdate() {

}
