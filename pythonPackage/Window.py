import pygame, threading, time, tkinter as tk
from ctypes import *
from enum import IntEnum
from .Framework import *
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
            reply = PlainReply()
            self.framework.SetCameraTransform(reply,
                Transform(
                    position = Vec2(x = 0, y = 0), 
                    orientation = 0,
                    scale = 13
                )
        )

    def onMouseButtonReleased(self, event):
        self.mouseButtons[event.num] = False


    def onMouseScroll(self, event):
        self.mouseScrollDelta = 1 if event.delta > 0 else -1

    def onFrameUpdate(self):

        if not self.framework.doesSceneExist:
            self.destroy()
            return

        prevMousePosition = self.mousePosition
        self.mousePosition = self.winfo_pointerxy()
        
        currX, currY = self.mousePosition 
        prevX, prevY = prevMousePosition

        deltaX, deltaY = (currX - prevX, currY - prevY)

        reply = PlainReply()
        self.framework.UpdateScreenSpaceCameraControlRig(
            ScreenSpaceControls(
                moveX = deltaX if self.mouseButtons[1] else 0,
                moveY = deltaY if self.mouseButtons[1] else 0,
                rotate = deltaX if self.mouseButtons[3] else 0,
                zoom = self.mouseScrollDelta
            )
        )
        self.after(9, self.onFrameUpdate)

        # reset for next frame  so we don't double use it
        self.mouseScrollDelta = 0

    def onClickedClearSceneButton(self):
        reply = PlainReply()
        self.framework.RemoveAllEntities(reply)

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