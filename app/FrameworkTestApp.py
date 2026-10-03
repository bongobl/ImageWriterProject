import pygame, threading, time, tkinter as tk
from ctypes import *
from ImageWriter import *

class WindowUI(tk.Tk):
    def __init__(self, name, windowSize):
        super().__init__()

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

        button = tk.Button(
            self, 
            text="Clear Scene",
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

    def onMouseButtonPressed(self, event):
        self.mouseButtons[event.num] = True

        if event.num == 2:
            statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
            imageWriter.SetCameraTransform(statusMessage,
                Transform(
                    positionX = 0, 
                    positionY = 0, 
                    scale = 7, 
                    angle = 0
                )
        )

    def onMouseButtonReleased(self, event):
        self.mouseButtons[event.num] = False


    def onMouseScroll(self, event):
        self.mouseScrollDelta = -1 if event.delta > 0 else 1

    def onFrameUpdate(self):

        if not imageWriter.doesSceneExist:
            self.destroy()
            return

        prevMousePosition = self.mousePosition
        self.mousePosition = self.winfo_pointerxy()
        
        currX, currY = self.mousePosition 
        prevX, prevY = prevMousePosition

        deltaX, deltaY = (currX - prevX, currY - prevY)

        statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
        imageWriter.TEMP_MoveCameraLocalSpace(statusMessage,
            Transform(
                positionX = deltaX if self.mouseButtons[1] else 0, 
                positionY = deltaY if self.mouseButtons[1] else 0, 
                scale = self.mouseScrollDelta, 
                angle = deltaX / 14 if self.mouseButtons[3] else 0)
        )
        self.after(9, self.onFrameUpdate)

        # reset for next frame  so we don't double use it
        self.mouseScrollDelta = 0

    def onClickedClearButton(self):
        statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
        imageWriter.RemoveAllEntities(statusMessage)

    def on_window_close(self):
    
        self.windowIsActive = False
    
        imageWriter.WaitForSceneToDispose()
        self.destroy()

def isWindowUIOpen():
    return windowUI.windowIsActive



def createSomeSampleShapes():

    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    input("Wait for a bit here for Window to initialize... Press Enter")

    cameraTransform = Transform()
    widthFromheight = ctypes.c_float(0.0)
    if not imageWriter.GetCameraTransform(statusMessage, cameraTransform, widthFromheight):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    else:
        print(f"camera scaleX = {cameraTransform.scale * widthFromheight.value}, scaleY = {cameraTransform.scale}")

    dimX = 8
    dimY = 6
    spacingX = 2.4
    spacingY = 1.3

    startX = (dimX - 1) / 2 * spacingX
    startY = (dimY - 1) / 2 * spacingY

    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddEllipse(statusMessage,
                Transform(positionX = j * spacingX - startX, positionY = i * spacingY - startY, scale = 1, angle = 0),
                Color(red = 1, green = 0, blue = 0, alpha = 1),
                Vec2(0.5, 0.35)
            ):
                print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(statusMessage):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    input("Press Enter")
    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddRectangle(statusMessage,
                Transform(positionX = j * spacingX - startX, positionY = i * spacingY - startY, scale = 1, angle = 0),
                Color(red = 0, green = 1, blue = 1, alpha = 1),
                Vec2(0.5, 0.35)
            ):
                print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(statusMessage):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    input("Press Enter")
    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddTriangle(statusMessage,
                Transform(positionX = j * spacingX - startX, positionY = i * spacingY - startY, scale = 1, angle = 0),
                Color(red = 1, green = 1, blue = 0, alpha = 1),
                Vec2(-0.5, 0.35), Vec2(-0.5, -0.35), Vec2(0.5, 0)
            ):
                print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(statusMessage):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = cameraTransform.scaleX, positionY = cameraTransform.scaleY, scaleX = 1, scaleY = 1, angle = 0), 
    #     Color(red = 1, green = 0, blue = 0, alpha = 1)
    # )

    # instance.AddRectangle(statusMessage, 
    #     Transform(positionX = -cameraTransform.scaleX, positionY = -cameraTransform.scaleY, scaleX = 3, scaleY = 2, angle = 20), 
    #     Color(red = 0.5, green = 0.5, blue = 1, alpha = 1)
    # )

    # instance.AddTriangle(statusMessage,
    # 	Transform(positionX = 0, positionY = 0, scaleX = 1, scaleY = 1, angle = 60),
    # 	Color(red = 0, green = 1, blue = 0, alpha = 1),
    #     Vec2(1,-1), Vec2( 0,2 ), Vec2(-1,-1)
    # )
    
    # input("Press Enter")
    # instance.RemoveAllEntities(statusMessage);

    # input("Press Enter")
    # instance.AddRectangle(statusMessage, 
    #     Transform(positionX = 0, positionY = 0, scaleX = cameraTransform.scaleX - 1, scaleY = cameraTransform.scaleY - 1, angle = -30), 
    #     Color(red = 0.5, green = 1, blue = 0.5, alpha = 0.35 )
    # )
    # instance.AddTriangle(statusMessage,
    # 	Transform(positionX = 0, positionY = 0, scaleX = 1, scaleY = 1, angle = 0),
    # 	Color(red = 1, green = 1, blue = 0, alpha = 1),
    #     Vec2(3,1), Vec2(1,3), Vec2(-2,0)
    # )
    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = 3, positionY = -2, scaleX = 3, scaleY = 2, angle = -10), 
    #     Color(red = 0, green = 0, blue = 1, alpha = 0.4)
    # )

    # input("Press Enter")
    # instance.RemoveAllEntities(statusMessage);

    # input("Press Enter")
    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = -5, positionY = 3, scaleX = 1, scaleY = 4, angle = -25), 
    #     Color(red = 0, green = 1, blue = 1, alpha = 0.6)
    # )

    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = 7, positionY = -4, scaleX = 0.5, scaleY = 1.5, angle = 60), 
    #     Color(red = 1, green = 1, blue = 0, alpha = 0.2)
    # )

    # instance.AddRectangle(statusMessage, 
    #     Transform(positionX = -8, positionY = -1, scaleX = 1, scaleY = 1, angle = 12), 
    #     Color(red = 1, green = 0.5, blue = 1, alpha = 0.6)
    # )



if __name__ == "__main__":

    # create UI
    windowUI = WindowUI(name = "My Test App", windowSize="1920x1080")   

    # create ImageWriter
    imageWriter = ImageWriter()
    imageWriter.Init(windowUI.canvasId, fnIsWindowOpen = isWindowUIOpen)
    
    apiThread = threading.Thread(target = createSomeSampleShapes)
    apiThread.start()

    # run UI
    windowUI.onFrameUpdate()
    windowUI.mainloop()
    
    print("Disposing scene")
    
    apiThread.join()
    # dispose
    imageWriter.Dispose()

    print("Disposing instance")