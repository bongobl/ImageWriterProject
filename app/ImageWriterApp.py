import socket, ctypes, threading, pygame, time, tkinter as tk
from ImageWriterProtocol import *
from ctypes import *
import errno


# Winsock Constant: Socket operation on non-socket
WSAENOTSOCK = 10038

class WindowUI(tk.Tk):
    def __init__(self, name, windowSize):
        super().__init__()

        self.title(name)
        self.geometry(windowSize)

        self.protocol("WM_DELETE_WINDOW", self.on_window_close)

        self.canvas = tk.Canvas(self, width=640, height=480, bg="black")
        self.canvas.pack(padx=50, pady=50, expand=True, fill=tk.BOTH)

        button = tk.Button(
            self, 
            text="TODO: Clear canvas",
            command=self.onClickedClearButton,
            font=("Helvetica", 16, "bold"),
            padx=5,
            pady=2
        )

        # 3. Position the button in the window
        button.pack(pady=20)

        # update to make canvas live and then obtain canvas id
        self.update()
        self.canvasId = self.canvas.winfo_id()

        self.windowIsActive = True

    def onClickedClearButton(self):
        print("TODO: Make this button clear the canvas!")

    def on_window_close(self):
    
        self.windowIsActive = False
    
        # wait some time for render thread to finish
        time.sleep(0.07) 
        self.destroy()

def isWindowUIOpen():
    return root.windowIsActive

# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

    def Init(self, windowHandle, fnIsWindowOpen, fnReceiveCommands):
        
        self.framework = ctypes.CDLL("./ImageWriterAPI.dll")

        self.framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
        self.framework.CreateImageWriterInstance.restype = ctypes.c_bool

        self.framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        self.framework.DrawCircle.restype = ctypes.c_bool

        self.framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        self.framework.DrawRectangle.restype = ctypes.c_bool

        self.framework.InitRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_int64]
        self.framework.InitRenderWindow.restype = ctypes.c_bool

        self.framework.UpdateRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_float]
        self.framework.UpdateRenderWindow.restype = ctypes.c_bool

        self.framework.DisposeRenderWindow.argtypes = [HImageWriterInstance]
        self.framework.DisposeRenderWindow.restype = ctypes.c_bool

        self.framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
        self.framework.DestroyImageWriterInstance.restype = ctypes.c_bool

        self.framework.IsIsolatedRenderWindowOpen.argtypes = [HImageWriterInstance]
        self.framework.IsIsolatedRenderWindowOpen.restype = ctypes.c_bool

        if not self.framework.CreateImageWriterInstance(ctypes.byref(self)):
            print("Failed to create image writer instance")
            exit(1)

        self.fnReceiveCommands = fnReceiveCommands
        self.imageWriterThread = threading.Thread(target = fnReceiveCommands, args=(self,))
        self.imageWriterThread.start()


        self.fnIsWindowOpen = fnIsWindowOpen

        # if there was no window handle, SFML will create its own isolated, so we must rely
        # on its event handling to tell us if the window is open
        if windowHandle == 0:
            self.fnIsWindowOpen = lambda: self.framework.IsIsolatedRenderWindowOpen(self)


        self.windowLoopThread = threading.Thread(target = self.runRenderWindow, args=(windowHandle, ))
        self.windowLoopThread.start()


    def runRenderWindow(self, windowHandle):

        self.framework.InitRenderWindow(self, windowHandle)

        clock = pygame.time.Clock()
        while self.fnIsWindowOpen():
            deltaSeconds = clock.tick(12) / 1000.0
            self.framework.UpdateRenderWindow(self, deltaSeconds)

        self.framework.DisposeRenderWindow(self)

    def Dispose(self):

        self.windowLoopThread.join()
        self.imageWriterThread.join()
        self.framework.DestroyImageWriterInstance(ctypes.byref(self))


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
            if 'connToClient' in globals():
                del connToClient

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
                            frameworkFunctionStatus = instance.framework.DrawCircle(instance, frameworkFunctionMessage, circleParams.centerX, circleParams.centerY, circleParams.radius);

                        case Command.DrawRectangle:

                            status, errorMessage = receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(DrawRectangleParams))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            # deserialize drawRectangle params
                            rectangleParams = DrawRectangleParams.from_buffer_copy(paramsBuffer)
                            print(f"From client: {rectangleParams.toString()}")
                            frameworkFunctionStatus = instance.framework.DrawRectangle(instance, frameworkFunctionMessage, rectangleParams.centerX, rectangleParams.centerY, rectangleParams.halfExtentX, rectangleParams.halfExtentY);

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

    # create UI
    root = WindowUI(name = "My Test App", windowSize="1920x1080")   

    # create ImageWriter
    sampleInstance = HImageWriterInstance()
    sampleInstance.Init(root.canvasId, fnIsWindowOpen = isWindowUIOpen, fnReceiveCommands = runNetworkService)
    
    # run UI
    root.mainloop()
    
    print("Disposing")
    
    # close sockets
    try:
        connToClient
    except NameError:
        print("Disposed cleanly - No client socket active")
    else:
        print("Warning: Disposing app while client socket was active")
        connToClient.close()
    serverSocket.close()

    # dispose
    sampleInstance.Dispose()
