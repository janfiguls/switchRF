# Echo client program
import socket

HOST = '192.168.56.1'    # The remote host
PORT = 5000              # The same port as used by the server
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'hola')
    data = s.recv(1024)
print('Received', repr(data))