import socket

# Define the host and port
host = '127.0.0.1'  # Localhost (can use an IP address or 'localhost')
port = 8080

# Create a socket object (IPv4 and TCP)
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address (host, port)
server_socket.bind((host, port))

# Start listening for incoming connections (max backlog of 5)
server_socket.listen(5)

print(f"Server listening on {host}:{port}...")

# Accept client connections
while True:
    client_socket, client_address = server_socket.accept()
    print(f"Connection from {client_address} established.")

    # Send a welcome message to the client
    client_socket.send(b"Welcome to the server!\n")

    # Receive data from the client
    data = client_socket.recv(1024)
    print(f"Received from client: {data.decode()}")

    # Close the client socket
    client_socket.close()
    print("Client connection closed.")
