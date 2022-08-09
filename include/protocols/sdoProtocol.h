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

    /**
     * @brief Sends data request to host.
     * 
     * @return true 
     * @return false 
     */
    bool sendDataRequest();

    /**
     * @brief Send data to the host.
     * 
     * @param data 
     * @param size 
     * @return true 
     * @return false 
     */
    bool sendData(uint8_t* data, uint8_t size);

    /**
     * @brief Add a callback function. Callback is called on data recieved from the host.
     * 
     * @param callback 
     */
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

    // recieve buffer
    uint8_t* _buffer;
    uint8_t _bufferSize;
    uint8_t _currentPos;

    // send buffer
    uint8_t* _data;
    uint8_t _dataSize;
    uint8_t _transmitted;

    std::vector<void(*)(uint8_t*, uint8_t)> _callbacks;

public:
    SDOHost(NCDevice* device, channel_t channel);
    ~SDOHost();

    /**
     * @brief Set the Data object
     * 
     * @param data 
     * @param size 
     * @return true 
     * @return false 
     */
    bool setData(uint8_t* data, uint8_t size);

    /**
     * @brief Send data to client
     * 
     * @return true 
     * @return false 
     */
    bool sendData();

    void addCallback(void(*callback)(uint8_t*, uint8_t));

    void onRecieve(uint8_t* data, uint8_t size);
    void onUpdate();
};