#pragma once

#include <algorithm>
#include <vector>

#include "protocol.h"

const uint8_t SDO_REQUEST_DATA = 1;
const uint8_t SDO_DATA_START = 2;
const uint8_t SDO_DATA_PACKET = 3;
const uint8_t SDO_DATA_END = 4;

const uint8_t SDO_MAX_PACKET_SIZE = 7;

struct SDOClient_t
{
    enum state {
        IDLE,
        WAITING,
        RECIEVING,
        SENDING,
    };
};


class SDOClient : public NCProtocol {
private:
    SDOClient_t::state _state;

    uint8_t* _recievedData;
    uint8_t _dataSize;
    uint8_t _currentPos;

    std::vector<void(*)(uint8_t*, uint8_t)> _callbacks;

public:
    SDOClient(NCDevice* device, channel_t channel);
    ~SDOClient();

    bool sendDataRequest();
    bool sendData(uint8_t* data, uint8_t size);

    void addCallback(void(*callback)(uint8_t*, uint8_t));

    void onRecieve(uint8_t* data, uint8_t size);
    void onUpdate();
};

struct SDOHost_t {
    enum state {
        IDLE,
        SEND_DATA,
        RECIEVING
    };
};


class SDOHost : public NCProtocol {
private:

    SDOHost_t::state _state;

    uint8_t _transmitted;

public:
    SDOHost(NCDevice* device, channel_t channel);
    ~SDOHost();

    uint8_t* _data;
    uint8_t _dataSize;

    void onRecieve(uint8_t* data, uint8_t size);
    void onUpdate();
};