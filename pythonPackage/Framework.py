import pygame, ctypes, threading, time, tkinter as tk
from ctypes import *
from .Common import *
from enum import IntEnum

class GridViewMode(IntEnum):
    NONE = 0
    FOREGROUND = 1
    BACKGROUND = 2

class RenderConfig(ctypes.Structure):
    _fields_ = [
        ("gridViewMode", ctypes.c_int),
    ]

    def __init__(self):
        self.gridViewMode = GridViewMode.BACKGROUND

class Framework(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

    doesSceneExist = False
    sceneInitializedCondition = threading.Condition()
    sceneDisposedCondition = threading.Condition()
    renderConfig = RenderConfig()

    def __init__(self):
        
        self.runtimeLib = ctypes.CDLL("./ImageWriterAPI.dll")

        self.runtimeLib.ImageWriter_Instance_Intialize.argtypes = [ctypes.POINTER(Framework)]
        self.runtimeLib.ImageWriter_Instance_Intialize.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Instance_Dispose.argtypes = [ctypes.POINTER(Framework)]
        self.runtimeLib.ImageWriter_Instance_Dispose.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_Init.argtypes = [Framework, ctypes.c_int64, RenderConfig]
        self.runtimeLib.ImageWriter_Scene_Init.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_Dispose.argtypes = [Framework]
        self.runtimeLib.ImageWriter_Scene_Dispose.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_UpdateFrame.argtypes = [Framework, ctypes.c_float]
        self.runtimeLib.ImageWriter_Scene_UpdateFrame.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_IsSelfManagedRenderWindowOpen.argtypes = [Framework]
        self.runtimeLib.ImageWriter_Scene_IsSelfManagedRenderWindowOpen.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_UpdateRenderConfig.argtypes = [Framework, RenderConfig]
        self.runtimeLib.ImageWriter_Scene_UpdateRenderConfig.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_SetCameraTransform.argtypes = [Framework, ctypes.c_char_p, Transform]
        self.runtimeLib.ImageWriter_Scene_SetCameraTransform.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_GetCameraTransform.argtypes = [Framework, ctypes.c_char_p, ctypes.POINTER(Transform), ctypes.POINTER(ctypes.c_float)]
        self.runtimeLib.ImageWriter_Scene_GetCameraTransform.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_TEMP_MoveCameraLocalSpace.argtypes = [Framework, ctypes.c_char_p, Transform]
        self.runtimeLib.ImageWriter_Scene_TEMP_MoveCameraLocalSpace.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_AddEntity.argtypes = [Framework, ctypes.c_char_p, AddEntityParams]
        self.runtimeLib.ImageWriter_Scene_AddEntity.restype = ctypes.c_bool

        self.runtimeLib.ImageWriter_Scene_RemoveAllEntities.argtypes = [Framework, ctypes.c_char_p]
        self.runtimeLib.ImageWriter_Scene_RemoveAllEntities.restype = ctypes.c_bool


    def Init(self, windowHandle, fnIsWindowOpen):
        if not self.runtimeLib.ImageWriter_Instance_Intialize(ctypes.byref(self)):
            print("Image writer: Failed to create image writer instance")
            exit(1)

        # set function to tell render loop when window is still open
        # Note: if there was no window handle, the runtimeLib will create its own self managed one, so we must rely
        # on its event handling to tell us window's open/closed state
        self.fnIsWindowOpen = fnIsWindowOpen
        if windowHandle == 0:
            self.fnIsWindowOpen = lambda: self.runtimeLib.ImageWriter_Scene_IsSelfManagedRenderWindowOpen(self)


        # spawn render thread
        self.renderThread = threading.Thread(target = self.initAndRunScene, args=(windowHandle, ))
        self.renderThread.start()

        with self.sceneInitializedCondition:
            self.sceneInitializedCondition.wait_for(lambda: self.doesSceneExist is True)

    def Dispose(self):

        self.renderThread.join()
        self.runtimeLib.ImageWriter_Instance_Dispose(ctypes.byref(self))
        print("Disposing instance")

    def initAndRunScene(self, windowHandle):

        self.runtimeLib.ImageWriter_Scene_Init(self, windowHandle, self.renderConfig)

        with self.sceneInitializedCondition:
            self.doesSceneExist = True
            self.sceneInitializedCondition.notify()

        clock = pygame.time.Clock()
        while self.fnIsWindowOpen():
            deltaSeconds = clock.tick(120) / 1000.0
            self.runtimeLib.ImageWriter_Scene_UpdateFrame(self, deltaSeconds)

        print("Disposing scene")
        self.runtimeLib.ImageWriter_Scene_Dispose(self)
        with self.sceneDisposedCondition:
            self.doesSceneExist = False
            self.sceneDisposedCondition.notify()

    def WaitForSceneToDispose(self):
        
        with self.sceneDisposedCondition:
             self.sceneDisposedCondition.wait_for(lambda: self.doesSceneExist is False)

    def UpdateRenderConfig(self):
        return self.runtimeLib.ImageWriter_Scene_UpdateRenderConfig(self, self.renderConfig)

    def SetCameraTransform(self, statusMessage, transform):
        return self.runtimeLib.ImageWriter_Scene_SetCameraTransform(self, statusMessage, transform)

    def GetCameraTransform(self, statusMessage, outTransform, outWidthFromHeight):
        return self.runtimeLib.ImageWriter_Scene_GetCameraTransform(self, statusMessage, ctypes.byref(outTransform), ctypes.byref(outWidthFromHeight))

    def TEMP_MoveCameraLocalSpace(self, statusMessage, transform):
        return self.runtimeLib.ImageWriter_Scene_TEMP_MoveCameraLocalSpace(self, statusMessage, transform)

    def AddEntity(self, statusMessage, params):
        return self.runtimeLib.ImageWriter_Scene_AddEntity(self, statusMessage, params)

    def RemoveAllEntities(self, statusMessage):
        return self.runtimeLib.ImageWriter_Scene_RemoveAllEntities(self, statusMessage)