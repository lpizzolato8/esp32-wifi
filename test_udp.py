import socket

ESP32_IP = "192.168.1.246" 
PORT = 3333

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(2.0)

sock.sendto(b"Hello ESP32", (ESP32_IP, PORT))

try:
    data, addr = sock.recvfrom(1024)
    print(f"Received raw binary: {data.hex()}")
except socket.timeout:
    print("No response received.")