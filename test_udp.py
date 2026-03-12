import socket
import struct

# --- CONFIGURATION ---
# Replace this with the IP you see in the ESP32 Serial Monitor
ESP32_IP = "192.168.0.239" 
PORT = 3333

# This format matches your C struct:
# <  : Little-endian
# H  : uint16_t (sync_word)
# B  : uint8_t  (type)
# H  : uint16_t (len)
# 64s: 8-bit array of length 64 (payload)
# H  : uint16_t (checksum)
STRUCT_FORMAT = "<H B H 64s H"

def send_binary(msg_type, float_value):
    # 1. Pack the float into 4 bytes
    # 'f' is for a standard 32-bit float
    binary_data = struct.pack("f", float_value)
    
    # 2. Pad the data to 64 bytes to match the C struct payload size
    payload = binary_data.ljust(64, b'\x00')
    
    # 3. Calculate a simple checksum (sum of all bytes in payload)
    checksum = sum(payload) & 0xFFFF
    
    # 4. Pack the full frame
    # Sync word is 0xAA55
    sync_word = 0xAA55
    data_len = len(binary_data) # This tells the ESP32 only 4 bytes are real
    
    packet = struct.pack(STRUCT_FORMAT, 
                         sync_word, 
                         msg_type, 
                         data_len, 
                         payload, 
                         checksum)

    # 5. Send via UDP
    print(f"Sending float {float_value} to {ESP32_IP}...")
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.sendto(packet, (ESP32_IP, PORT))
        print("Packet sent!")
        print(f"Raw Hex: {packet.hex().upper()}")

if __name__ == "__main__":
    # Test sending a float (45.67) as Message Type 1
    send_binary(msg_type=1, float_value=45.67)