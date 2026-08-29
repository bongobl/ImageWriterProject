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

                    # read message from client
                    try:
                        rawDataReceived = connToClient.recv(MAX_RECV_BUFFER_SIZE)

                    # will fail if client disconnects without gracefully telling us
                    # return to listening state
                    except ConnectionResetError as e:
                        print(f"ConnectionResetError: {e}\n\n")
                        break

                    # error check message
                    if not rawDataReceived:
                        print("Server error: failed to receive message from client\n\n")
                        break

                    # display message in console
                    messageReceived = rawDataReceived.decode('utf-8')
                    print(f"Server: Received data: {messageReceived}")

                    # a message value of "Close" means client gracefully disconnected
                    # return to listening state
                    if messageReceived == "Close":
                        print(f"Server: Client at {clientAddr} gracefully disconnected\n\n")
                        break

                    # sent reply to client
                    reply = "Nice to hear from you lol!"

                    try:
                        connToClient.sendall(reply.encode('utf-8'))
                    except ConnectionResetError as e:
                        print(f"ConnectionResetError: {e}\n\n")
                        break


# main
if __name__ == "__main__":
    runServer()

