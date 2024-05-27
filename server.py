import socket
import os
def save_image(data, image_path):
    os.path.join(image_path,"image.jpg")
    with open(image_path, 'wb') as f:
        f.write(data)
def main():
    # server IP and port number
    host = "127.0.0.1"
    port = 80
    image_path = "./tmp-image"
    if not os.path.isdir(image_path):
        os.mkdir(image_path)
    # creat socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # bind IP and port
    server_socket.bind((host, port))
    print("bind success")
    # start listening,the most waiting number of connection is 5
    server_socket.listen(5)
    print(f"Server is listening on {host}:{port}")

    try:
        while True:
            # accept client to connect
            client_socket, client_address = server_socket.accept()
            print(f"Connection from {client_address}")

            # recieve data from client
            data = b""
            while True:
                chunk = client_socket.recv(1024)
                if not chunk:
                    break
                data += chunk    
            if data:
                print(f"Received image, saving image...")
                save_image(data, image_path)
                print(f"Image save successfully!")
                # reply to client
                client_socket.sendall(b"Server received your message.")

            # close connection
            client_socket.close()

    except KeyboardInterrupt:
        print("Server stopped.")
    finally:
        # close server socket
        server_socket.close()

if __name__ == "__main__":
    print("???")
    main()