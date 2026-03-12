#include "wifi_helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;
static const char *TAG = "wifi_station";
static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {                     // is wifi driver working?
        esp_wifi_connect();                                                                 // search for network and attempt connection
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {       // has it tried to connect and/or failed
        if (s_retry_num < 5) {                                                              // uses counter to check how many times we have failed to connect
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retrying connection...");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);                          // triggers fail bit
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {                 // network connection successful?? DHCP server assied IP to ESP32???
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);                         // trigger connection bit
    }
}

void wifi_init_sta(void) {

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));



    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "The Glitch",
            .password = "Lotsofheart",

            //for both
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

           .sae_pwe_h2e = WPA3_SAE_PWE_BOTH, //WPA3 support

           .pmf_cfg = {
            .capable = true,    // PMF for WPA3
            .required = false   // No need for WPA2
        },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}