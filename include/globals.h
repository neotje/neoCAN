#pragma once

#include <Arduino.h>

#define toCanId(nodeId, channel) ((nodeId << 18) | channel)
#define getNodeIdFromCanId(canId) (canId >> 18)
#define getChannelFromCanId(canId) (canId & 0x3FFFF)

typedef uint16_t nodeId_t;
typedef uint16_t channel_t;