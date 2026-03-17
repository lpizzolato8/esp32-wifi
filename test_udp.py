import socket
import struct
import time

# CONFIG
# Replace this with the IP you see in the ESP32 Serial Monitor

ESP32_IP = "192.168.1.246" 
PORT = 3333
STRUCT_FORMAT = "<H B H 64s H"

def send_binary(msg_type, float_value):
    # Pack the float into 4 bytes
    binary_data = struct.pack("f", float_value)
    
    # Pad the data to 64 bytes
    payload = binary_data.ljust(64, b'\x00')
    
    # Calculate checksum
    checksum = sum(payload) & 0xFFFF
    
    # Pack the full frame
    sync_word = 0xAA55
    data_len = len(binary_data)
    
    packet = struct.pack(STRUCT_FORMAT, 
                         sync_word, 
                         msg_type, 
                         data_len, 
                         payload, 
                         checksum)

    # Use a socket with a timeout for the response
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(1.0)
        
        print(f"Sending float {float_value} to {ESP32_IP}...")
        
        # Capture start time
        start_time = time.perf_counter()
        
        sock.sendto(packet, (ESP32_IP, PORT))
        
        try:
            # Wait for echo from ESP32
            data, addr = sock.recvfrom(1024)
            
            # Capture end time
            end_time = time.perf_counter()
            
            # Calculate latency in milliseconds
            latency = (end_time - start_time) * 1000
            print(f"Latency: {latency:.3f} ms")
            
        except socket.timeout:
            print("Error: Latency measurement failed (Timeout)")


# Test sending a value as Message Type 1
    # type 1 = float 
    # type 2 = int 
    # type 3 = char array

if __name__ == "__main__":
    send_binary(msg_type=1, float_value=45.67)


    