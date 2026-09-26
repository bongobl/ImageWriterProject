import pygame, threading, tkinter as tk
from ctypes import *
from ImageWriterCommon import *


class ImageWriter(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

    def __init__(self):
        
        self.framework = ctypes.CDLL("./ImageWriterAPI.dll")

        self.framework.ImageWriter_Instance_Intialize.argtypes = [ctypes.POINTER(ImageWriter)]
        self.framework.ImageWriter_Instance_Intialize.restype = ctypes.c_bool

        self.framework.ImageWriter_Instance_Dispose.argtypes = [ctypes.POINTER(ImageWriter)]
        self.framework.ImageWriter_Instance_Dispose.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_Init.argtypes = [ImageWriter, ctypes.c_int64]
        self.framework.ImageWriter_Scene_Init.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_Dispose.argtypes = [ImageWriter]
        self.framework.ImageWriter_Scene_Dispose.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_UpdateFrame.argtypes = [ImageWriter, ctypes.c_float]
        self.framework.ImageWriter_Scene_UpdateFrame.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_IsSelfManagedRenderWindowOpen.argtypes = [ImageWriter]
        self.framework.ImageWriter_Scene_IsSelfManagedRenderWindowOpen.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_GetCameraTransform.argtypes = [ImageWriter, ctypes.c_char_p, ctypes.POINTER(Transform)]
        self.framework.ImageWriter_Scene_GetCameraTransform.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_AddEllipse.argtypes = [ImageWriter, ctypes.c_char_p, Transform, Color]
        self.framework.ImageWriter_Scene_AddEllipse.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_AddRectangle.argtypes = [ImageWriter, ctypes.c_char_p, Transform, Color]
        self.framework.ImageWriter_Scene_AddRectangle.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_AddTriangle.argtypes = [ImageWriter, ctypes.c_char_p, Transform, Color, Vec2, Vec2, Vec2]
        self.framework.ImageWriter_Scene_AddTriangle.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_RemoveAllEntities.argtypes = [ImageWriter, ctypes.c_char_p]
        self.framework.ImageWriter_Scene_RemoveAllEntities.restype = ctypes.c_bool


    def Init(self, windowHandle, fnIsWindowOpen, fnReceiveCommands):
        if not self.framework.ImageWriter_Instance_Intialize(ctypes.byref(self)):
            print("Image writer: Failed to create image writer instance")
            exit(1)

        # set function to tell render loop when window is still open
        # Note: if there was no window handle, framework will create its own self managed one, so we must rely
        # on its event handling to tell us window's open/closed state
        self.fnIsWindowOpen = fnIsWindowOpen
        if windowHandle == 0:
            self.fnIsWindowOpen = lambda: self.framework.ImageWriter_Scene_IsSelfManagedRenderWindowOpen(self)


        # spawn render thread
        self.renderThread = threading.Thread(target = self.initAndRunScene, args=(windowHandle, ))
        self.renderThread.start()

        # TODO: use thread condition to block here until InitScene has finished in the render thread so we 
        # don't end up submitting commands to a non-existet scene. We could then probably remove the command 
        # receiver thread member here and have it maintained by the app.

        # span command receiver thread
        self.fnReceiveCommands = fnReceiveCommands
        self.commandReceiverThread = threading.Thread(target = fnReceiveCommands, args=(self,))
        self.commandReceiverThread.start()


    def initAndRunScene(self, windowHandle):

        self.framework.ImageWriter_Scene_Init(self, windowHandle)

        clock = pygame.time.Clock()
        while self.fnIsWindowOpen():
            deltaSeconds = clock.tick(120) / 1000.0
            self.framework.ImageWriter_Scene_UpdateFrame(self, deltaSeconds)

        self.framework.ImageWriter_Scene_Dispose(self)

    def Dispose(self):

        self.renderThread.join()
        self.commandReceiverThread.join()
        self.framework.ImageWriter_Instance_Dispose(ctypes.byref(self))

    def GetCameraTransform(self, statusMessage, cameraView):
        return self.framework.ImageWriter_Scene_GetCameraTransform(self, statusMessage, ctypes.byref(cameraView))

    def AddEllipse(self, statusMessage, transform, color):
        return self.framework.ImageWriter_Scene_AddEllipse(self, statusMessage, transform, color)

    def AddRectangle(self, statusMessage, transform, color):
        return self.framework.ImageWriter_Scene_AddRectangle(self, statusMessage, transform, color)

    def AddTriangle(self, statusMessage, transform, color, point1, point2, point3):
        return self.framework.ImageWriter_Scene_AddTriangle(self, statusMessage, transform, color, point1, point2, point3)

    def RemoveAllEntities(self, statusMessage):
        return self.framework.ImageWriter_Scene_RemoveAllEntities(self, statusMessage)