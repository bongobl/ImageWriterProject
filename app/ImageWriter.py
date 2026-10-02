import pygame, threading, time, tkinter as tk
from ctypes import *
from ImageWriterCommon import *


class ImageWriter(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

    doesSceneExist = False
    sceneInitializedCondition = threading.Condition()
    sceneDisposedCondition = threading.Condition()

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

        self.framework.ImageWriter_Scene_SetCameraTransform.argtypes = [ImageWriter, ctypes.c_char_p, Transform]
        self.framework.ImageWriter_Scene_SetCameraTransform.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_GetCameraTransform.argtypes = [ImageWriter, ctypes.c_char_p, ctypes.POINTER(Transform)]
        self.framework.ImageWriter_Scene_GetCameraTransform.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_TEMP_MoveCameraLocalSpace.argtypes = [ImageWriter, ctypes.c_char_p, Transform]
        self.framework.ImageWriter_Scene_TEMP_MoveCameraLocalSpace.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_AddEllipse.argtypes = [ImageWriter, ctypes.c_char_p, Transform, Color, Vec2]
        self.framework.ImageWriter_Scene_AddEllipse.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_AddRectangle.argtypes = [ImageWriter, ctypes.c_char_p, Transform, Color, Vec2]
        self.framework.ImageWriter_Scene_AddRectangle.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_AddTriangle.argtypes = [ImageWriter, ctypes.c_char_p, Transform, Color, Vec2, Vec2, Vec2]
        self.framework.ImageWriter_Scene_AddTriangle.restype = ctypes.c_bool

        self.framework.ImageWriter_Scene_RemoveAllEntities.argtypes = [ImageWriter, ctypes.c_char_p]
        self.framework.ImageWriter_Scene_RemoveAllEntities.restype = ctypes.c_bool


    def Init(self, windowHandle, fnIsWindowOpen):
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

        with self.sceneInitializedCondition:
            self.sceneInitializedCondition.wait_for(lambda: self.doesSceneExist is True)


    def initAndRunScene(self, windowHandle):

        self.framework.ImageWriter_Scene_Init(self, windowHandle)

        with self.sceneInitializedCondition:
            self.doesSceneExist = True
            self.sceneInitializedCondition.notify()

        clock = pygame.time.Clock()
        while self.fnIsWindowOpen():
            deltaSeconds = clock.tick(120) / 1000.0
            self.framework.ImageWriter_Scene_UpdateFrame(self, deltaSeconds)

        self.framework.ImageWriter_Scene_Dispose(self)
        with self.sceneDisposedCondition:
            self.doesSceneExist = False
            self.sceneDisposedCondition.notify()

    def Dispose(self):

        self.renderThread.join()
        self.framework.ImageWriter_Instance_Dispose(ctypes.byref(self))

    def WaitForSceneToDispose(self):
        
        with self.sceneDisposedCondition:
             self.sceneDisposedCondition.wait_for(lambda: self.doesSceneExist is False)

    def SetCameraTransform(self, statusMessage, transform):
        return self.framework.ImageWriter_Scene_SetCameraTransform(self, statusMessage, transform)

    def GetCameraTransform(self, statusMessage, outTransform):
        return self.framework.ImageWriter_Scene_GetCameraTransform(self, statusMessage, ctypes.byref(outTransform))

    def TEMP_MoveCameraLocalSpace(self, statusMessage, transform):
        return self.framework.ImageWriter_Scene_TEMP_MoveCameraLocalSpace(self, statusMessage, transform)

    def AddEllipse(self, statusMessage, transform, color, halfExtents):
        return self.framework.ImageWriter_Scene_AddEllipse(self, statusMessage, transform, color, halfExtents)

    def AddRectangle(self, statusMessage, transform, color, halfExtents):
        return self.framework.ImageWriter_Scene_AddRectangle(self, statusMessage, transform, color, halfExtents)

    def AddTriangle(self, statusMessage, transform, color, point1, point2, point3):
        return self.framework.ImageWriter_Scene_AddTriangle(self, statusMessage, transform, color, point1, point2, point3)

    def RemoveAllEntities(self, statusMessage):
        return self.framework.ImageWriter_Scene_RemoveAllEntities(self, statusMessage)