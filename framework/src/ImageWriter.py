import ctypes, pygame, threading, tkinter as tk
from ctypes import *


# TODO: Mirror API.h: move to common header
class ImageWriter(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

    def Init(self, windowHandle, fnIsWindowOpen, fnReceiveCommands):
        
        self.framework = ctypes.CDLL("./ImageWriterAPI.dll")

        self.framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(ImageWriter)]
        self.framework.CreateImageWriterInstance.restype = ctypes.c_bool

        self.framework.DrawCircle.argtypes = [ImageWriter, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        self.framework.DrawCircle.restype = ctypes.c_bool

        self.framework.DrawRectangle.argtypes = [ImageWriter, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        self.framework.DrawRectangle.restype = ctypes.c_bool

        self.framework.InitRenderWindow.argtypes = [ImageWriter, ctypes.c_int64]
        self.framework.InitRenderWindow.restype = ctypes.c_bool

        self.framework.UpdateRenderWindow.argtypes = [ImageWriter, ctypes.c_float]
        self.framework.UpdateRenderWindow.restype = ctypes.c_bool

        self.framework.DisposeRenderWindow.argtypes = [ImageWriter]
        self.framework.DisposeRenderWindow.restype = ctypes.c_bool

        self.framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(ImageWriter)]
        self.framework.DestroyImageWriterInstance.restype = ctypes.c_bool

        self.framework.IsIsolatedRenderWindowOpen.argtypes = [ImageWriter]
        self.framework.IsIsolatedRenderWindowOpen.restype = ctypes.c_bool

        if not self.framework.CreateImageWriterInstance(ctypes.byref(self)):
            print("Failed to create image writer instance")
            exit(1)

        self.fnReceiveCommands = fnReceiveCommands
        self.commandReceiverThread = threading.Thread(target = fnReceiveCommands, args=(self,))
        self.commandReceiverThread.start()


        self.fnIsWindowOpen = fnIsWindowOpen

        # if there was no window handle, SFML will create its own isolated, so we must rely
        # on its event handling to tell us if the window is open
        if windowHandle == 0:
            self.fnIsWindowOpen = lambda: self.framework.IsIsolatedRenderWindowOpen(self)


        self.renderThread = threading.Thread(target = self.runRenderWindow, args=(windowHandle, ))
        self.renderThread.start()


    def runRenderWindow(self, windowHandle):

        self.framework.InitRenderWindow(self, windowHandle)

        clock = pygame.time.Clock()
        while self.fnIsWindowOpen():
            deltaSeconds = clock.tick(12) / 1000.0
            self.framework.UpdateRenderWindow(self, deltaSeconds)

        self.framework.DisposeRenderWindow(self)

    def Dispose(self):

        self.renderThread.join()
        self.commandReceiverThread.join()
        self.framework.DestroyImageWriterInstance(ctypes.byref(self))

    def DrawCircle(self, statusMessage, centerX, centerY, radius):
        return self.framework.DrawCircle(self, statusMessage, centerX, centerY, radius)

    def DrawRectangle(self, statusMessage, centerX, centerY, halfExtentX, halfExtentY):
        return self.framework.DrawRectangle(self, statusMessage, centerX, centerY, halfExtentX, halfExtentY)