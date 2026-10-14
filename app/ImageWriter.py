import pygame, threading, time, tkinter as tk
from ctypes import *
from ImageWriterCommon import *
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



class Window(tk.Tk):

    def __init__(self, name, windowSize):
        super().__init__()

        self.framework = Framework()

        self.title(name)
        self.geometry(windowSize)

        self.protocol("WM_DELETE_WINDOW", self.on_window_close)

        self.canvas = tk.Canvas(self, width=640, height=480, bg="black")
        self.canvas.pack(padx=50, pady=50, expand=True, fill=tk.BOTH)

        self.mousePosition = self.winfo_pointerxy()
        self.mouseButtons = {1:False, 2: False, 3: False}
        self.mouseScrollDelta = 0

        # register mouse events
        for i in range(1,4):
            self.canvas.bind(f"<Button-{i}>", self.onMouseButtonPressed)
            self.canvas.bind(f"<ButtonRelease-{i}>", self.onMouseButtonReleased)
        
        self.canvas.bind("<MouseWheel>", self.onMouseScroll)

        self.bottomFrame = tk.Frame(self, width=150)
        self.bottomFrame.pack(pady=20)

        # Clear scene button
        self.clearSceneButton = tk.Button(
            self.bottomFrame, 
            text="Clear Scene",
            command=self.onClickedClearSceneButton,
            font=("Helvetica", 16, "bold"),
            padx=5,
            pady=2
        )
        self.clearSceneButton.grid(row=0, column=0)

        # Grid view mode label
        self.gridViewLabel = tk.Label(self.bottomFrame, text="Grid view:", font=("Helvetica", 16, "bold"))
        self.gridViewLabel.grid(row=0, column=1, padx = (100, 5))

        # Grid view mode option menu
        self.gridViewModeOptions = ["None", "Foreground", "Background"]
        
        self.gridViewModeSelection = tk.StringVar(self.bottomFrame)
        self.gridViewModeSelection.set(self.gridViewModeOptions[self.framework.renderConfig.gridViewMode])
        self.gridViewModeSelection.trace_add("write", self.onUpdatedGridViewModeOptionMenu)

        self.gridViewModeOptionMenu = tk.OptionMenu(
            self.bottomFrame, 
            self.gridViewModeSelection,
            *self.gridViewModeOptions
        )
        self.gridViewModeOptionMenu.config(width=12, font=("Helvetica", 16))
        self.gridViewModeOptionMenu["menu"].config(font=("Helvetica", 16))
        self.gridViewModeOptionMenu.grid(row=0, column=2, padx=5)

        # update to make canvas live and then obtain canvas id
        self.update()
        self.canvasId = self.canvas.winfo_id()

        self.windowIsActive = True

        self.framework.Init(self.canvasId, fnIsWindowOpen = self.isWindowUIOpen)

    def cleanUp(self):
        self.framework.Dispose()

    def onMouseButtonPressed(self, event):
        self.mouseButtons[event.num] = True

        if event.num == 2:
            statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
            self.framework.SetCameraTransform(statusMessage,
                Transform(
                    position = Vec2(x = 0, y = 0), 
                    orientation = 0,
                    scale = 13
                )
        )

    def onMouseButtonReleased(self, event):
        self.mouseButtons[event.num] = False


    def onMouseScroll(self, event):
        self.mouseScrollDelta = -1 if event.delta > 0 else 1

    def onFrameUpdate(self):

        if not self.framework.doesSceneExist:
            self.destroy()
            return

        prevMousePosition = self.mousePosition
        self.mousePosition = self.winfo_pointerxy()
        
        currX, currY = self.mousePosition 
        prevX, prevY = prevMousePosition

        deltaX, deltaY = (currX - prevX, currY - prevY)

        statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
        self.framework.TEMP_MoveCameraLocalSpace(statusMessage,
            Transform(
                position = Vec2(x = deltaX if self.mouseButtons[1] else 0, y = deltaY if self.mouseButtons[1] else 0), 
                orientation = deltaX / 14 if self.mouseButtons[3] else 0,
                scale = self.mouseScrollDelta
            )
        )
        self.after(9, self.onFrameUpdate)

        # reset for next frame  so we don't double use it
        self.mouseScrollDelta = 0

    def onClickedClearSceneButton(self):
        statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
        self.framework.RemoveAllEntities(statusMessage)

    def onUpdatedGridViewModeOptionMenu(self, *args):

        for i in range(len(self.gridViewModeOptions)):
            if self.gridViewModeOptions[i] == self.gridViewModeSelection.get():
                self.framework.renderConfig.gridViewMode = i
                self.framework.UpdateRenderConfig()
                break


    def on_window_close(self):
    
        self.windowIsActive = False
    
        self.framework.WaitForSceneToDispose()
        self.destroy()

    def isWindowUIOpen(self):
        return self.windowIsActive