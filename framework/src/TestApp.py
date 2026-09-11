import ctypes, pygame, threading, tkinter as tk
from ctypes import *

framework = ctypes.CDLL("./ImageWriterAPI.dll")


# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

def cString(pyString):
    charArray = ctypes.create_string_buffer(pyString.encode('utf-8'))
    ptrToFirstChar = cast(charArray, c_char_p)
    return ptrToFirstChar

def runImageWriterFlow(instance: HImageWriterInstance):

    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    framework.DrawCircle(instance, statusMessage, 350, 200, 100)
    framework.DrawRectangle(instance, statusMessage, 1500, 700, 200, 150)


    framework.DrawCircle(instance, statusMessage, 400, 200, 80);
    framework.DrawRectangle(instance, statusMessage, 100, 150, 75, 120)

    framework.DrawCircle(instance, statusMessage, 800, 300, 70)
    framework.DrawCircle(instance, statusMessage, 300, 550, 150)
    framework.DrawRectangle(instance, statusMessage, 500, 300, 400, 10)


def main_loop_callback():

    global clock
    deltaSeconds = clock.tick(120) / 1000.0
    framework.UpdateRenderWindow(instance, deltaSeconds)

    root.after(1, main_loop_callback)

if __name__ == "__main__":

    framework = ctypes.CDLL("./ImageWriterAPI.dll")

    framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.CreateImageWriterInstance.restype = ctypes.c_bool

    framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawCircle.restype = ctypes.c_bool

    framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawRectangle.restype = ctypes.c_bool

    framework.InitRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_int64]
    framework.InitRenderWindow.restype = ctypes.c_bool

    framework.UpdateRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_float]
    framework.UpdateRenderWindow.restype = ctypes.c_bool

    framework.DisposeRenderWindow.argtypes = [HImageWriterInstance]
    framework.DisposeRenderWindow.restype = ctypes.c_bool
    
    framework.TEMP_IsRenderWindowOpen.argtypes = [HImageWriterInstance]
    framework.TEMP_IsRenderWindowOpen.restype = ctypes.c_bool

    framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.DestroyImageWriterInstance.restype = ctypes.c_bool


    instance = HImageWriterInstance()

    if not framework.CreateImageWriterInstance(ctypes.byref(instance)):
        print("Failed to create image writer instance")
        exit(1)

    imageWriterThread = threading.Thread(target = runImageWriterFlow, args=(instance,))
    imageWriterThread.start()

    root = tk.Tk()
    root.title("My App")
    root.geometry("1920x1080")

    canvas = tk.Canvas(root, width=640, height=480, bg="black")
    canvas.pack(padx=50, pady=50, expand=True, fill=tk.BOTH)

    button = tk.Button(
        root, 
        text="Click Me", 
        font=("Helvetica", 16, "bold"),
        padx=5,
        pady=2
    )

    # 3. Position the button in the window
    button.pack(pady=20)


    root.update()
    canvasId = canvas.winfo_id()

    framework.InitRenderWindow(instance, canvasId)
    root.after(1, main_loop_callback)

    clock = pygame.time.Clock()
    root.mainloop()
    
    print("Disposing")
    framework.DisposeRenderWindow(instance)

    imageWriterThread.join()
    framework.DestroyImageWriterInstance(ctypes.byref(instance))
