#pragma once

#include <stdint.h>
#include "lwip/sockets.h"

#pragma pack(push, 1)
typedef struct {
    uint16_t sync_word;    // 0xAA55
    uint8_t  type;         // Message type ID
    uint16_t len;          // Length of data in payload
    uint8_t  payload[64];  // The actual data
    uint16_t checksum;     // Simple sum of payload bytes
} binary_frame_t;
#pragma pack(pop)

#define FRAME_MAGIC 0xAABB

void udp_receive_task(void *pvParameters);
