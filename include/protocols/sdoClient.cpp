#include "sdoProtocol.h"

SDOClient::SDOClient(NCDevice *device, channel_t channel) : NCProtocol(device, channel) {
    _state = SDOClient_t::IDLE;
}

SDOClient::~SDOClient() {
}

bool SDOClient::sendDataRequest() {
    if(_state == SDOClient_t::IDLE && sendPacket((uint8_t *)&SDO_REQUEST_DATA, 1)) {
        _state = SDOClient_t::WAITING;
        return true;
    }
    return false;
}

void SDOClient::onRecieve(uint8_t *data, uint8_t size) {
    if (size < 1) return;

    uint8_t command = data[0];

    if (command == SDO_DATA_START && _state == SDOClient_t::WAITING) {
        _dataSize = data[1];
        _currentPos = 0;
        _state = SDOClient_t::RECIEVING;
        
        // allocate memory for the data
        delete[] _recievedData;
        _recievedData = new uint8_t[_dataSize + 1];
        memset(_recievedData, 0, _dataSize + 1);
    } else if (command == SDO_DATA_PACKET && _state == SDOClient_t::RECIEVING) {
        // copy the data to the recievedData
        for (size_t i = 1; i < size; i++) {
            _recievedData[_currentPos] = data[i];
            _currentPos++;
        }
    } else if (command == SDO_DATA_END && _state == SDOClient_t::RECIEVING) {
        _state = SDOClient_t::IDLE;
        Serial.print((char *)_recievedData);
        Serial.print(_dataSize);
        Serial.print(" ");
        Serial.println(strlen((char *)_recievedData));
    }
}

void SDOClient::onUpdate() {

}
