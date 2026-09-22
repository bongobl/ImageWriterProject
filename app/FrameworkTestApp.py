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

        button = tk.Button(
            self, 
            text="Clear Image",
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
        statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)
        imageWriter.ClearImage(statusMessage)

    def on_window_close(self):
    
        self.windowIsActive = False
    
        # wait some time for render thread to finish
        time.sleep(0.07) 
        self.destroy()

def isWindowUIOpen():
    return windowUI.windowIsActive



def createSomeSampleShapes(instance: ImageWriter):

    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    input("Wait for a bit here for Window to initialize... Press Enter")

    cameraTransform = Transform()
    instance.GetCameraTransform(statusMessage, cameraTransform)
    print(f"camera scaleX = {cameraTransform.scaleX}, scaleY = {cameraTransform.scaleY}")

    instance.AddEllipse(statusMessage, 
        Transform(positionX = cameraTransform.scaleX, positionY = cameraTransform.scaleY, scaleX = 1, scaleY = 1, angle = 0), 
        Color(red = 1, green = 0, blue = 0, alpha = 1)
    )

    instance.AddRectangle(statusMessage, 
        Transform(positionX = -cameraTransform.scaleX, positionY = -cameraTransform.scaleY, scaleX = 3, scaleY = 2, angle = 20), 
        Color(red = 0.5, green = 0.5, blue = 1, alpha = 1)
    )

    instance.AddTriangle(statusMessage,
		Vec2(1,-1), Vec2( 0,2 ), Vec2(-1,-1),
		Transform(positionX = 0, positionY = 0, scaleX = 1, scaleY = 1, angle = 60),
		Color(red = 0, green = 1, blue = 0, alpha = 1)
    )
    
    input("Press Enter")
    instance.ClearImage(statusMessage);

    input("Press Enter")
    instance.AddRectangle(statusMessage, 
        Transform(positionX = 0, positionY = 0, scaleX = cameraTransform.scaleX - 1, scaleY = cameraTransform.scaleY - 1, angle = -30), 
        Color(red = 0.5, green = 1, blue = 0.5, alpha = 0.35 )
    )
    instance.AddTriangle(statusMessage,
		Vec2(3,1), Vec2(1,3), Vec2(-2,0),
		Transform(positionX = 0, positionY = 0, scaleX = 1, scaleY = 1, angle = 0),
		Color(red = 1, green = 1, blue = 0, alpha = 1)
    )
    instance.AddEllipse(statusMessage, 
        Transform(positionX = 3, positionY = -2, scaleX = 3, scaleY = 2, angle = -10), 
        Color(red = 0, green = 0, blue = 1, alpha = 0.4)
    )

    input("Press Enter")
    instance.ClearImage(statusMessage);

    input("Press Enter")
    instance.AddEllipse(statusMessage, 
        Transform(positionX = -5, positionY = 3, scaleX = 1, scaleY = 4, angle = -25), 
        Color(red = 0, green = 1, blue = 1, alpha = 0.6)
    )

    instance.AddEllipse(statusMessage, 
        Transform(positionX = 7, positionY = -4, scaleX = 0.5, scaleY = 1.5, angle = 60), 
        Color(red = 1, green = 1, blue = 0, alpha = 0.2)
    )

    instance.AddRectangle(statusMessage, 
        Transform(positionX = -8, positionY = -1, scaleX = 1, scaleY = 1, angle = 12), 
        Color(red = 1, green = 0.5, blue = 1, alpha = 0.6)
    )

    input("Press Enter")
    instance.ClearImage(statusMessage);


if __name__ == "__main__":

    # create UI
    windowUI = WindowUI(name = "My Test App", windowSize="1920x1080")   

    # create ImageWriter
    imageWriter = ImageWriter()
    imageWriter.Init(windowUI.canvasId, fnIsWindowOpen = isWindowUIOpen, fnReceiveCommands = createSomeSampleShapes)
    
    # run UI
    windowUI.mainloop()
    
    print("Disposing")
    
    # dispose
    imageWriter.Dispose()