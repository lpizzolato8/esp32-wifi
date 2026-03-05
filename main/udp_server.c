#include "udp_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/sockets.h"
/*struct sockaddr_in {
    sa_family_t    sin_family;   // Address family (2 bytes)
    in_port_t      sin_port;     // Port number (2 bytes)
    struct in_addr sin_addr;     // IPv4 address (4 bytes)
    char           sin_zero[8];  // Padding to match generic struct size (8 bytes)
};
*/

#define PORT 3333
#define RX_BUFFER_SIZE 128
static const char *TAG = "udp_server";

void udp_receive_task(void *pvParameters) {
    
    char rx_buffer[RX_BUFFER_SIZE];                // Allocates an array of 128 bytes to store the payload of incoming UDP packets
    struct sockaddr_in dest_addr;                  // Declares a structure (IPv4 address,port info)      

    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Configures the socket to listen on all available network interfaces
    dest_addr.sin_family = AF_INET;                // Specifies that the address family is IPv4 (AF_INET)
    dest_addr.sin_port = htons(PORT);              // Sets the listening port to 3333, converting to network byte order

    while (1) {

        // delcare socket 
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {                                                             // has socket been created, if not then return error
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        if (bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {     //can bind? -> binds the socket to port and address if not error
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);                
            close(sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "Socket bound, port %d", PORT);                               // logs a message for socket listening 

        while (1) {
            struct sockaddr_storage source_addr;                                    // structure for storage (IPaddress, port)
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            if (len < 0) {                                                          //network error???
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            } else {
                rx_buffer[len] = 0;
                // Optimization: Avoid printf in high-speed loops; use ESP_LOG
                ESP_LOGI(TAG, "Received: %s", rx_buffer);
            }
        }

        if (sock != -1) {                                                           // cleans if inner loops return an error
            shutdown(sock, 0);
            close(sock);
        }
    }
}

int send_framed_binary(int sock, struct sockaddr_in *dest_addr, uint8_t msg_id, const uint8_t *payload, uint16_t length) {
    size_t total_size = sizeof(binary_frame_header_t) + length;
    uint8_t *tx_buffer = (uint8_t *)malloc(total_size);
    
    if (tx_buffer == NULL) {
        return -1; 
    }

    binary_frame_header_t *header = (binary_frame_header_t *)tx_buffer;
    header->magic_bytes = htons(FRAME_MAGIC); 
    header->message_id = msg_id;
    header->payload_len = htons(length); 

    if (payload != NULL && length > 0) {
        memcpy(tx_buffer + sizeof(binary_frame_header_t), payload, length);
    }

    // Prints the binary buffer to the monitor in hexadecimal format
    esp_log_buffer_hex(TAG, tx_buffer, total_size);

    int err = sendto(sock, tx_buffer, total_size, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
    free(tx_buffer);

    return err;
}