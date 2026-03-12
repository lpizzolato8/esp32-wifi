# ESP32 UDP Binary Receiver

A lightweight, high-performance UDP server for the ESP32 (ESP-IDF) designed to receive and parse raw binary data. This project features a robust custom-framed protocol with **WPA3-SAE** support and data integrity validation.

## 🚀 Features
- **Custom Binary Protocol**: Uses packed C structs to eliminate memory padding and minimize overhead.
- **WPA3 & WPA2 Support**: Configured for modern Wi-Fi security (SAE/PMF) while maintaining backward compatibility.
- **Data Integrity**: Implements a Sync Word (`0xAA55`) and 16-bit Checksum to ensure packets are complete and uncorrupted.
- **Non-Blocking Task**: Runs as a background FreeRTOS task, allowing the main application to handle other logic.

---

## 🛠 Binary Protocol Definition

The system communicates using a fixed-length frame. The C struct is packed using `#pragma pack(1)` to ensure the byte alignment matches exactly between Python (sender) and C (receiver).

| Field         | Type       | Size     | Description                                     |
|--------------|------------|----------|-------------------------------------------------|
| **Sync Word** | `uint16_t` | 2 Bytes  | Always `0xAA55` to identify the start of a frame. |
| **Msg Type**  | `uint8_t`  | 1 Byte   | ID to determine data type (e.g., 1=Float, 2=Int). |
| **Length**    | `uint16_t` | 2 Bytes  | Number of active data bytes in the payload.     |
| **Payload**   | `uint8_t[]`| 64 Bytes | The raw binary data container.                  |
| **Checksum**  | `uint16_t` | 2 Bytes  | Sum of all payload bytes for error detection.   |

---

## Project Structure

- `main/udp_server.c`: UDP listener task and binary parsing logic.
- `main/udp_server.h`: Binary struct definition and function prototypes.
- `main/wifi_helper.c`: Wi-Fi station initialization (WPA3/WPA2).
- `scripts/send_test.py`: Python script to pack and send test packets.

---

## Setup & Installation

### 1. Requirements
- ESP-IDF v5.x+
- Python 3.x

### 2. Configure Wi-Fi
Update your credentials in `main/wifi_helper.c`:
```c
#define ESP_WIFI_SSID      "Your_SSID"
#define ESP_WIFI_PASS      "Your_Password"
```
Code above found at lines 6 and 7

### 3. Build and Flash ESP32
<img width="110" height="41" alt="image" src="https://github.com/user-attachments/assets/746bfac0-9bb8-45ff-bb03-de168acd696f" />

### 4. Run python and Monitor ESP32


