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
    
    uint8_t rx_buffer[sizeof(binary_frame_t) + 10];
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
            } 
            
            // check if we received enough bytes to at least fill our struct
            if (len >= sizeof(binary_frame_t)) {
                
                // Map the buffer memory directly onto our struct
                binary_frame_t *pkg = (binary_frame_t *)rx_buffer;

                // Step 1: Check for the Sync Word. 
                // Note: 0xAA55 in Python might arrive as 0x55AA due to "Endianness"
                if (pkg->sync_word == 0xAA55 || pkg->sync_word == 0x55AA) {
                    
                    // Step 2: Extract data. 
                    // Since it's binary, we use memcpy to copy bytes into a variable.
                    float val;
                    memcpy(&val, pkg->payload, sizeof(float));
                    
                    ESP_LOGI(TAG, "Valid Packet Type: %d, Data: %.2f", pkg->type, val);
                } else {
                    ESP_LOGW(TAG, "Received junk or wrong sync word: 0x%04X", pkg->sync_word);
                }
            }
        }
        // Cleanup if the inner loop fails
        close(sock);
    }
}