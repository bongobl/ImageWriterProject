import socket, ctypes, threading
from ImageWriterProtocol import *
from ctypes import *


# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

def runNetworkService(instance: HImageWriterInstance):

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
                    
                    frameworkFunctionStatus = False
                    frameworkFunctionMessage = ctypes.create_string_buffer(b"Command ran successfully", MAX_REPLY_MESSAGE_LENGTH)
                    match command:
                        
                        case Command.SetupImage:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(SetupImageParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            
                            # deserialize drawCircle params
                            setupImageParams = SetupImageParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {setupImageParams.toString()}")
                            frameworkFunctionStatus = framework.SetupImage(instance, frameworkFunctionMessage, setupImageParams.width, setupImageParams.height)

                        case Command.DrawCircle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(DrawCircleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize drawCircle params
                            circleParams = DrawCircleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {circleParams.toString()}")
                            frameworkFunctionStatus = framework.DrawCircle(instance, frameworkFunctionMessage, circleParams.centerX, circleParams.centerY, circleParams.radius);

                        case Command.DrawRectangle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(DrawRectangleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize drawRectangle params
                            rectangleParams = DrawRectangleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {rectangleParams.toString()}")
                            frameworkFunctionStatus = framework.DrawRectangle(instance, frameworkFunctionMessage, rectangleParams.centerX, rectangleParams.centerY, rectangleParams.halfExtentX, rectangleParams.halfExtentY);

                        case Command.ExportImage:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(ExportImageParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize exportImage params
                            exportImageParams = ExportImageParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {exportImageParams.toString()}")
                            frameworkFunctionStatus = framework.ExportImage(instance, frameworkFunctionMessage, cast(exportImageParams.imageName, c_char_p));

                        case Command.Disconnecting:
                            
                            print(f"Client at {clientAddr} gracefully disconnected\n\n")
                            break
                            
                        case _:
                            print("Unrecognized command")


                    reply = Reply(status = frameworkFunctionStatus, message = frameworkFunctionMessage.value)
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

    # load image writer library and functions
    framework = ctypes.CDLL("C:\\Dev\\Practice\\BasicClaude\\ImageWriter\\buildGNU\\ImageWriterAPI.dll")

    framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.CreateImageWriterInstance.restype = ctypes.c_bool

    framework.SetupImage.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    framework.SetupImage.restype = ctypes.c_bool

    framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawCircle.restype = ctypes.c_bool

    framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawRectangle.restype = ctypes.c_bool

    framework.ExportImage.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_char_p]
    framework.ExportImage.restype = ctypes.c_bool

    framework.TEMP_RunSFMLWindow.argtypes = [HImageWriterInstance, ctypes.c_char_p]
    framework.TEMP_RunSFMLWindow.restype = ctypes.c_bool
    
    framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.DestroyImageWriterInstance.restype = ctypes.c_bool

    # create instance
    instance = HImageWriterInstance()
    if not framework.CreateImageWriterInstance(ctypes.byref(instance)):
        print("Failed to create image writer instance")
        exit(1)

    networkServiceThread = threading.Thread(target = runNetworkService, args=(instance,))
    networkServiceThread.start()
    
    frameworkFunctionMessage = ctypes.create_string_buffer(b"Command ran successfully", MAX_REPLY_MESSAGE_LENGTH)

    framework.TEMP_RunSFMLWindow(instance, frameworkFunctionMessage)
    
    networkServiceThread.join()

    # Todo: figure out where to put this when we find a way to gracefully kill the server
    framework.DestroyImageWriterInstance(instance)

