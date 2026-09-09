import ctypes
from ctypes import *
import threading

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

    input("Press ENTER to continue...")
    framework.SetupImage(instance, statusMessage, 1920, 1080)
    framework.DrawCircle(instance, statusMessage, 350, 200, 100)
    framework.DrawRectangle(instance, statusMessage, 1500, 700, 200, 150)
    framework.ExportImage(instance, statusMessage, cString("ImageLarge"))


    input("Press ENTER to continue...")
    framework.SetupImage(instance, statusMessage, 640, 480);
    framework.DrawCircle(instance, statusMessage, 400, 200, 80);
    framework.DrawRectangle(instance, statusMessage, 100, 150, 75, 120)
    framework.ExportImage(instance, statusMessage, cString("ImageSmall"))

    input("Press ENTER to continue...")
    framework.SetupImage(instance, statusMessage, 1280, 720)
    framework.DrawCircle(instance, statusMessage, 800, 300, 70)
    framework.DrawCircle(instance, statusMessage, 300, 550, 150)
    framework.DrawRectangle(instance, statusMessage, 500, 300, 400, 10)
    framework.ExportImage(instance, statusMessage, cString("ImageMedium"))

if __name__ == "__main__":
    framework = ctypes.CDLL("./ImageWriterAPI.dll")

    framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.CreateImageWriterInstance.restype = ctypes.c_bool

    framework.SetupImage.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    framework.SetupImage.restype = ctypes.c_bool

    framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawCircle.restype = ctypes.c_bool

    framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    framework.DrawRectangle.restype = ctypes.c_bool

    framework.ExportImage.argtypes = [HImageWriterInstance, ctypes.c_char_p, ctypes.c_char_p]
    framework.ExportImage.restype = ctypes.c_bool

    framework.InitRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_char_p]
    framework.InitRenderWindow.restype = ctypes.c_bool

    framework.UpdateRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_char_p]
    framework.UpdateRenderWindow.restype = ctypes.c_bool

    framework.DisposeRenderWindow.argtypes = [HImageWriterInstance, ctypes.c_char_p]
    framework.DisposeRenderWindow.restype = ctypes.c_bool
    
    framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
    framework.DestroyImageWriterInstance.restype = ctypes.c_bool


    instance = HImageWriterInstance()


    if not framework.CreateImageWriterInstance(ctypes.byref(instance)):
        print("Failed to create image writer instance")
        exit(1)

    imageWriterThread = threading.Thread(target = runImageWriterFlow, args=(instance,))
    imageWriterThread.start()

    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    framework.InitRenderWindow(instance, statusMessage)
    framework.UpdateRenderWindow(instance, statusMessage)
    framework.DisposeRenderWindow(instance, statusMessage)

    imageWriterThread.join()
    framework.DestroyImageWriterInstance(ctypes.byref(instance))
