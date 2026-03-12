# ESP32 UDP Binary Receiver
---

## Binary Protocol Definition

The system communicates using a fixed length frame. The C struct is packed using `#pragma pack(1)` to ensure the byte alignment matches exactly between Python (sender) and C (receiver).

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

### 2. Configure Wi-Fi and IP

Update your credentials in `main/wifi_helper.c`:
```c
.sta = {
.ssid = " SSID ",
.password = " Password",
```
Code above found at lines 53 and 54



Update the credentials of the ESP32 in `test_udp.py`:
```c
ESP32_IP = "192.168.x.xxx" 
PORT = 3333
```
Code above found at lines 6 and 7

IP Address of ESP32 can be found in the monitor section after connection. **STEP 4.**

<img width="681" height="35" alt="image" src="https://github.com/user-attachments/assets/ae6445ea-81d3-4479-98b6-999543d33228" />

### 3. Build and Flash ESP32

Build using the wrench and flash using the lighting bolt

<img width="110" height="41" alt="image" src="https://github.com/user-attachments/assets/746bfac0-9bb8-45ff-bb03-de168acd696f" />

### 4. Run python and Monitor ESP32

Run the python script and click on the monitor below to see the output

<img width="67" height="44" alt="image" src="https://github.com/user-attachments/assets/72e6bad2-b273-4001-8c54-63649045aa8e" />

### 5. Results

<img width="785" height="43" alt="image" src="https://github.com/user-attachments/assets/9276ad48-bbbe-4808-bb3f-fbc94b3e041d" />

