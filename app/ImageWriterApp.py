import socket, ctypes, threading, pygame, tkinter as tk
from ImageWriterProtocol import *
from ctypes import *
import errno


# Winsock Constant: Socket operation on non-socket
WSAENOTSOCK = 10038

# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

def runNetworkService(instance: HImageWriterInstance):

    global connToClient
    global serverSocket

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
            
            try:
                connToClient, clientAddr = serverSocket.accept()
            except OSError as e:

                # Check if the error is due to the socket being closed or invalidated
                if e.errno in (errno.EBADF, errno.EINVAL) or getattr(e, 'winerror', None) == WSAENOTSOCK:
                    print("Listening socket was closed. returning from runNetworkService().")
                    return
                else:
                    # This is a real, unexpected network or system error
                    raise e

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


def main_loop_callback():

    deltaSeconds = clock.tick(120) / 1000.0
    framework.UpdateRenderWindow(instance, deltaSeconds)

    root.after(1, main_loop_callback)

def onClickedClearButton():
    print("TODO: Make this button clear the canvas!")
# main
if __name__ == "__main__":

    # load image writer library and functions
    framework = ctypes.CDLL("./ImageWriterAPI.dll")

    framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.CreateImageWriterInstance.restype = ctypes.c_bool

    framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawCircle.restype = ctypes.c_bool

    framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawRectangle.restype = ctypes.c_bool

    framework.InitRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_int64]
    framework.InitRenderWindow.restype = ctypes.c_bool

    framework.UpdateRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_float]
    framework.UpdateRenderWindow.restype = ctypes.c_bool

    framework.DisposeRenderWindow.argtypes = [HImageWriterInstance]
    framework.DisposeRenderWindow.restype = ctypes.c_bool
    
    framework.TEMP_IsRenderWindowOpen.argtypes = [HImageWriterInstance]
    framework.TEMP_IsRenderWindowOpen.restype = ctypes.c_bool

    framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.DestroyImageWriterInstance.restype = ctypes.c_bool

    # create instance
    instance = HImageWriterInstance()

    if not framework.CreateImageWriterInstance(ctypes.byref(instance)):
        print("Failed to create image writer instance")
        exit(1)

    networkServiceThread = threading.Thread(target = runNetworkService, args=(instance,))
    networkServiceThread.start()
    
    root = tk.Tk()
    root.title("Image Writer App")
    root.geometry("1920x1080")

    canvas = tk.Canvas(root, width=640, height=480, bg="black")
    canvas.pack(padx=50, pady=50, expand=True, fill=tk.BOTH)

    button = tk.Button(
        root, 
        text="TODO: Clear canvas",
        command=onClickedClearButton,
        font=("Helvetica", 16, "bold"),
        padx=5,
        pady=2
    )

    # 3. Position the button in the window
    button.pack(pady=20)


    root.update()
    canvasId = canvas.winfo_id()

    framework.InitRenderWindow(instance, canvasId)
    root.after(1, main_loop_callback)

    clock = pygame.time.Clock()
    root.mainloop()

    framework.DisposeRenderWindow(instance)

    # close sockets
    try:
        connToClient
    except NameError:
        print("Variable is NOT defined.")
    else:
        print("Variable IS defined.")
        connToClient.close()
    serverSocket.close()
    
    networkServiceThread.join()

    
    # Todo: figure out where to put this when we find a way to gracefully kill the server
    framework.DestroyImageWriterInstance(instance)

