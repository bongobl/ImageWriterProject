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

                    # wait here and receive message from client
                    commandBuffer = bytearray()
                    status, errorMessage = receiveMessage(buffer = commandBuffer, connection = connToClient, size = COMMAND_SIZE)
                    if not status:
                        # print error message and return to listening state
                        print(errorMessage)
                        break

                    command = Command(int.from_bytes(commandBuffer))

                    # wait here and receive message from client
                    paramsBuffer = bytearray()
                    
                    
                    match command:
                        
                        case Command.InitImage:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(InitImageParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize drawCircle params
                            initImageParams = InitImageParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {initImageParams.toString()}")

                        case Command.DrawCircle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(DrawCircleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize drawCircle params
                            circleParams = DrawCircleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {circleParams.toString()}")

                        case Command.DrawRectangle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(DrawRectangleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize drawRectangle params
                            rectangleParams = DrawRectangleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {rectangleParams.toString()}")

                        case Command.ExportImage:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(ExportImageParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize exportImage params
                            exportImageParams = ExportImageParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {exportImageParams.toString()}")

                        case Command.Disconnecting:
                            
                            print(f"Client at {clientAddr} gracefully disconnected\n\n")
                            break
                            
                        case _:
                            print("Unrecognized command")


                    # create a dummy client message
                    reply = DummyReply(length = 24.6, width = 35.6, height = 32.1, resources = 56)
                    print(f"To client: {reply.toString()}")

                    # serialize client message
                    replyBuffer = bytes(reply)

                    # send reply to client
                    try:
                        connToClient.sendall(replyBuffer)
                    except ConnectionResetError as e:
                        print(f"ConnectionResetError: {e}\n\n")
                        break


# main
if __name__ == "__main__":

    runServer()

