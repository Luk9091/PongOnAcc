import socket
from time import sleep


class TcpServer:
    def __init__(self, server_addr = "10.42.0.1", server_port = 4242, buf_size = 200):
        self.SERVER_ADDR = server_addr
        self.SERVER_PORT = server_port
        self.BUF_SIZE = buf_size

        self.start()

    def start(self):
        self.sock = socket.socket()
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((self.SERVER_ADDR, self.SERVER_PORT))
        self.sock.listen(1)
        print("Server listening on", self.SERVER_ADDR, self.SERVER_PORT)

        # Wait for the client
        print("Waiting for client")
        self.con = None
        self.con, addr = self.sock.accept()
        print("Client connected from", addr)

    def read(self) -> tuple[float, float, float]:
        self.con.send(b"next")
        buf = self.con.recv(self.BUF_SIZE)
        buf = buf.decode("utf-8")
        table = buf.split(" ")
        return (float(table[0]), float(table[1]), float(table[2]))

    
    def stop(self):
        self.con.close()
        self.sock.close()


if __name__ == "__main__":
    server = TcpServer()
    server.start()
    
    try:
        while True:
            print(server.read())
            sleep(1)
    except KeyboardInterrupt:
        server.stop()
    
