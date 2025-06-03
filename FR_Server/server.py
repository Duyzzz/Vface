import socket
import threading
from network_address import pythonSv_endpoint

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def send_data(data, destination):
    print(f"Sending {len(data)} bytes of data to {destination}")
    sender_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sender_sock.connect(destination)
    # print("data type" + str(type(data)))
    # Check if data is a string and encode it, or send raw bytes directly
    if isinstance(data, str):
        sender_sock.sendall(data.encode("utf-8"))  # Encode if data is a string
    elif isinstance(data, (bytes, bytearray)):
        sender_sock.sendall(
            data
        )  # Send directly if data is already bytes or bytearray
    sender_sock.close()


def receive_image():
    try:
        print("Receiving image...")
        receiver_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        receiver_sock.bind(pythonSv_endpoint)
        receiver_sock.listen(1)
        conn, addr = receiver_sock.accept()
        print(f"Connection from {addr}")
        data = b""
        while True:
            chunk = conn.recv(1024)
            if not chunk:  # No more data, connection closed
                break
            data += chunk
        conn.close()
        return True, data
    except Exception as e:
        print(f"An error occurred while receiving the image: {e}")
        return False, None
    

class Receiver:
    def __init__(self, endpoint = pythonSv_endpoint):
        self.endpoint = endpoint
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind(self.endpoint)
        self.server_socket.listen(1)
        self.data_received = b""
        print(f"Receiver listening on {self.endpoint}")
    def start(self):
        print("Receiver started.")
        thread = threading.Thread(target=self.receive_data)
        try:
            thread.start()
            print("Receiver thread started.")
        except Exception as e:
            print(f"Error starting receiver thread: {e}")
    def receive_data(self):
        while True:
            conn, addr = self.server_socket.accept()
            print(f"Connection from {addr}")
            data = b""
            while True:
                chunk = conn.recv(1024)
                if not chunk:
                    break
                data += chunk
            conn.close()
            if not data:
                continue
            self.data_received = data
            # Process the received data
            # print(f"Received data: {data}")
            print(f"Received {len(data)} bytes of data, in endpoint: {self.endpoint}")
    def get_data(self):
        if not self.data_received:
            return None
        return self.data_received
    def reset_data(self):
        self.data_received = b""
    def stop(self):
        self.server_socket.close()
        print("Receiver stopped.")
    def receive_image(self):
        try:
            print("Receiving image...")
            conn, addr = self.server_socket.accept()
            print(f"Connection from {addr}")
            data = b""
            while True:
                chunk = conn.recv(1024)
                if not chunk:  # No more data, connection closed
                    break
                data += chunk
            conn.close()
            return True, data
        except Exception as e:
            print(f"An error occurred while receiving the image: {e}")
            return False, None

