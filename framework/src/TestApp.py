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
    return root.windowIsActive



def createSomeSampleShapes(instance: ImageWriter):

    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    time.sleep(0.5)
    instance.DrawCircle(statusMessage, 350, 200, 100)
    instance.DrawRectangle(statusMessage, 1500, 700, 200, 150)

    time.sleep(0.5)
    instance.DrawCircle(statusMessage, 400, 200, 80);
    instance.DrawRectangle(statusMessage, 100, 150, 75, 120)

    time.sleep(0.5)
    instance.DrawCircle(statusMessage, 800, 300, 70)
    instance.DrawCircle(statusMessage, 300, 550, 150)
    instance.DrawRectangle(statusMessage, 500, 300, 400, 10)


if __name__ == "__main__":
    
    # create UI
    root = WindowUI(name = "My Test App", windowSize="1920x1080")   

    # create ImageWriter
    imageWriter = ImageWriter()
    imageWriter.Init(root.canvasId, fnIsWindowOpen = isWindowUIOpen, fnReceiveCommands = createSomeSampleShapes)
    
    # run UI
    root.mainloop()
    
    print("Disposing")
    
    # dispose
    imageWriter.Dispose()