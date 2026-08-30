import tcpCommon, socket
from tcpCommon import *
def runServer():

    # set up listening socket (IPv4 + TCP)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket:

        # Prevent "Address already in use" errors upon restart
        serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        serverSocket.bind((HOST_IP, PORT))
        serverSocket.listen()

        # continually come back here to listen for new clients after previous ones disconnect
        while True:

            print(f"Server: listening on {HOST_IP}:{PORT}...")

            # listening state: wait here until a client connects
            connToClient, clientAddr = serverSocket.accept()

            with connToClient:
                print(f"Server: Connected to a client at {clientAddr}")

                # continually listen for new messages from client
                while True:

                    bufferValid = True
                    serverMessageBuffer = bytearray()

                    # read server message from client
                    try:
                        while len(serverMessageBuffer) < SERVER_MESSAGE_SIZE:
                            fragmentReceived = connToClient.recv(SERVER_MESSAGE_SIZE - len(serverMessageBuffer))

                            # error check on data received
                            if not fragmentReceived:
                                print("Server error: failed to receive data from client\n\n")
                                bufferValid = False
                                break
                            
                            serverMessageBuffer.extend(fragmentReceived)

                        # sanity check buffer
                        # return to listening state
                        if not bufferValid:
                            break

                    # will fail if client disconnects without gracefully telling us
                    # return to listening state
                    except (ConnectionResetError, ConnectionAbortedError) as e:
                        print(f"recv error: {e}\n\n")
                        break
                    
                    # deserialize server message and print
                    serverMessage = ServerMessage.from_buffer_copy(serverMessageBuffer)
                    print(f"Out: {serverMessage.toString()}")
                    
                    # a playerName of "Close" means client gracefully disconnected
                    # return to listening state
                    if serverMessage.playerName.decode('utf-8') == "Close":
                        print(f"Server: Client at {clientAddr} gracefully disconnected\n\n")
                        break

                    # create a dummy client message
                    clientMessage = ClientMessage(length = 24.6, width = 35.6, height = 32.1, resources = 56)
                    print(f"In: {clientMessage.toString()}")

                    # serialize client message
                    clientMessageBuffer = bytes(clientMessage)

                    # send message to client
                    try:
                        connToClient.sendall(clientMessageBuffer)
                    except ConnectionResetError as e:
                        print(f"ConnectionResetError: {e}\n\n")
                        break


# main
if __name__ == "__main__":
    runServer()

