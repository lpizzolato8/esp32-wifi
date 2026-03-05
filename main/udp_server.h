#pragma once

#include <stdint.h>
#include "lwip/sockets.h"

#pragma pack(push, 1)
typedef struct {
    uint16_t magic_bytes;
    uint8_t  message_id;
    uint16_t payload_len;
} binary_frame_header_t;
#pragma pack(pop)

#define FRAME_MAGIC 0xAABB

void udp_receive_task(void *pvParameters);
int send_framed_binary(int sock, struct sockaddr_in *dest_addr, uint8_t msg_id, const uint8_t *payload, uint16_t length);