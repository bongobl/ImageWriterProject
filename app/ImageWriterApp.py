import socket, threading, time, errno, ImageWriter, tkinter as tk
from ctypes import *
from ImageWriterProtocol import *
import ImageWriter as iw

# Winsock Constant: Socket operation on non-socket
WSAENOTSOCK = 10038

def runNetworkService():

    global connToClient
    global serverSocket

    framework = window.framework

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
            if 'connToClient' in globals():
                del connToClient

            try:
                connToClient, clientAddr = serverSocket.accept()
            except OSError as e:

                # Check if the error is due to the socket being intentionally closed or invalidated
                # from another thread
                if e.errno in (errno.EBADF, errno.EINVAL) or getattr(e, 'winerror', None) == WSAENOTSOCK:
                    print("Listening socket was closed. exiting service loop.")
                    return
                else:
                    # This is a real, unexpected network or system error
                    raise e

            with connToClient:
                print(f"Server: Connected to a client at {clientAddr}")

                # continually listen for new messages from client
                while True:

                    # wait here and receive command from client
                    commandBuffer = bytearray()
                    status, errorMessage = receiveMessage(buffer = commandBuffer, connection = connToClient, size = COMMAND_SIZE)
                    if not status:
                        # print error message and return to listening state
                        print(errorMessage)
                        break

                    command = Command(int.from_bytes(commandBuffer))

                    # wait here and receive message from client
                    paramsBuffer = bytearray()
                    
                    frameworkFunctionMessage = ctypes.create_string_buffer(b"Command ran successfully", MAX_REPLY_MESSAGE_LENGTH)
                    match command:
                        
                        case Command.SetCameraTransform:
                            
                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(SetCameraTransformParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize addEllipse params
                            params = SetCameraTransformParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {params}")

                            frameworkFunctionSucceeded = framework.SetCameraTransform(frameworkFunctionMessage, params.transform)

                            commandStatus = Status(success = frameworkFunctionSucceeded, message = frameworkFunctionMessage.value)
                            reply = PlainReply(status = commandStatus)

                        case Command.GetCameraTransform:

                            print("From client: Getting the camera transform")
                            cameraTransform = iw.Transform()
                            widthFromheight = ctypes.c_float(0.0)
                            frameworkFunctionSucceeded = framework.GetCameraTransform(frameworkFunctionMessage, cameraTransform, widthFromheight)

                            commandStatus = Status(success = frameworkFunctionSucceeded, message = frameworkFunctionMessage.value)
                            reply = TransformReply(
                                status = commandStatus, 
                                transform = cameraTransform, 
                                cameraWidthFromHeight = widthFromheight
                            )

                        case Command.RemoveAllEntities:

                            print("From client: removing all entities from scene")
                            frameworkFunctionSucceeded = framework.RemoveAllEntities(frameworkFunctionMessage)


                            commandStatus = Status(success = frameworkFunctionSucceeded, message = frameworkFunctionMessage.value)
                            reply = PlainReply(status = commandStatus)

                        case Command.AddEllipse:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(AddEllipseParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize addEllipse params
                            ellipseParams = AddEllipseParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {ellipseParams}")

                            frameworkFunctionSucceeded = framework.AddEntity(
                                frameworkFunctionMessage,
                                iw.AddEntityParams(
                                    entityType = iw.EntityType.ELLIPSE, 
                                    transform = ellipseParams.transform,
                                    color = ellipseParams.color,
                                    ellipse = iw.AddEllipseParams(halfExtents = ellipseParams.halfExtents)
                                )
                            )

                            commandStatus = Status(success = frameworkFunctionSucceeded, message = frameworkFunctionMessage.value)
                            reply = PlainReply(status = commandStatus)

                        case Command.AddRectangle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(AddRectangleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize addRectangle params
                            rectangleParams = AddRectangleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {rectangleParams}")

                            frameworkFunctionSucceeded = framework.AddEntity(
                                frameworkFunctionMessage,
                                iw.AddEntityParams(
                                    entityType = iw.EntityType.RECTANGLE, 
                                    transform = rectangleParams.transform,
                                    color = rectangleParams.color,
                                    rectangle = iw.AddRectangleParams(halfExtents = rectangleParams.halfExtents)
                                )
                            )
                            
                            commandStatus = Status(success = frameworkFunctionSucceeded, message = frameworkFunctionMessage.value)
                            reply = PlainReply(status = commandStatus)
                        
                        case Command.AddTriangle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(AddTriangleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize addRectangle params
                            triangleParams = AddTriangleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {triangleParams}")
 
                            frameworkFunctionSucceeded = framework.AddEntity(
                                frameworkFunctionMessage,
                                iw.AddEntityParams(
                                    entityType = iw.EntityType.TRIANGLE, 
                                    transform = triangleParams.transform,
                                    color = triangleParams.color,
                                    triangle = iw.AddTriangleParams(
                                        point1 = triangleParams.point1,
                                        point2 = triangleParams.point2,
                                        point3 = triangleParams.point3,
                                    )
                                )
                            )
                            
                            commandStatus = Status(success = frameworkFunctionSucceeded, message = frameworkFunctionMessage.value)
                            reply = PlainReply(status = commandStatus)

                        case Command.Disconnecting:
                            
                            print(f"Client at {clientAddr} gracefully disconnected\n\n")

                            # break out of client listening loop and go back to listening for
                            # any new clients listening on port
                            connToClient.close()
                            break
                            
                        case _:
                            print("Unrecognized command")
                            reply = PlainReply(
                                success = False,
                                message = ("Unrecognized command sent to ImageWriterApp over socket").encode('utf-8')
                            )


                    print(f"To client: {reply}")

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

    # create UI
    window = iw.Window(name = "ImageWriter App", windowSize="1920x1480")   

    serviceThread = threading.Thread(target = runNetworkService)
    serviceThread.start()

    # run UI
    window.onFrameUpdate()
    window.mainloop()
    
    # close sockets
    try:
        connToClient
    except NameError:
        print("Disposed cleanly - No client socket active")
    else:
        print("Warning: Disposing app while client socket was active")
        connToClient.close()
    serverSocket.close()

    serviceThread.join()

    # dispose
    window.cleanUp()
