import ctypes, pygame, threading, time, tkinter as tk
from ctypes import *

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


def createSomeSampleShapes(instance: HImageWriterInstance):

    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    time.sleep(0.5)
    instance.framework.DrawCircle(instance, statusMessage, 350, 200, 100)
    instance.framework.DrawRectangle(instance, statusMessage, 1500, 700, 200, 150)

    time.sleep(0.5)
    instance.framework.DrawCircle(instance, statusMessage, 400, 200, 80);
    instance.framework.DrawRectangle(instance, statusMessage, 100, 150, 75, 120)

    time.sleep(0.5)
    instance.framework.DrawCircle(instance, statusMessage, 800, 300, 70)
    instance.framework.DrawCircle(instance, statusMessage, 300, 550, 150)
    instance.framework.DrawRectangle(instance, statusMessage, 500, 300, 400, 10)


if __name__ == "__main__":
    
    # create UI
    root = WindowUI(name = "My Test App", windowSize="1920x1080")   

    # create ImageWriter
    sampleInstance = HImageWriterInstance()
    sampleInstance.Init(root.canvasId, fnIsWindowOpen = isWindowUIOpen, fnReceiveCommands = createSomeSampleShapes)
    
    # run UI
    root.mainloop()
    
    print("Disposing")
    
    # dispose
    sampleInstance.Dispose()