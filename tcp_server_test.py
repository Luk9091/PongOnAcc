#!/usr/bin/python

import socket
from time import sleep

# Set server address to machines IP
SERVER_ADDR = "10.42.0.1"

# These constants should match the client
BUF_SIZE = 200
TEST_ITERATIONS = 10
SERVER_PORT = 4242

# Open socket to the server
sock = socket.socket()
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((SERVER_ADDR, SERVER_PORT))
sock.listen(1)
print("server listening on", SERVER_ADDR, SERVER_PORT)

# Wait for the client
try:
    con = None
    con, addr = sock.accept()
    print("client connected from", addr)
except:
    sock.close()
    exit(1)

# Repeat test for a number of iterations
try:
    while True:
        write_len = con.send(b"Hello from server")
        buf = con.recv(BUF_SIZE)
        print(buf.decode("utf-8"))
        sleep(0.3)
except:
    pass
# All done
con.close()
sock.close()
print("test completed")
