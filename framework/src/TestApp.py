import ctypes
from ctypes import *

framework = ctypes.CDLL("./ImageWriterAPI.dll")

# Copied from ImageWriterProtocol for help testing
class Reply(ctypes.Structure):
    _fields_ = [
        ("status", ctypes.c_bool),
        ("message", ctypes.c_char * 256),
    ]

    def toString(self):
        return f"Reply: status = {self.status}, message = {self.message.decode('utf-8')}"


# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

def cString(pyString):
    charArray = ctypes.create_string_buffer(pyString.encode('utf-8'))
    ptrToFirstChar = cast(charArray, c_char_p)
    return ptrToFirstChar

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

framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
framework.DestroyImageWriterInstance.restype = ctypes.c_bool


instance = HImageWriterInstance()


if not framework.CreateImageWriterInstance(ctypes.byref(instance)):
    print("Failed to create image writer instance")
    exit(1)



statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

imageName = "MyPythonImage"
framework.SetupImage(instance, statusMessage, 1280, 720);


framework.DrawCircle(instance, statusMessage, 800, 300, 70);
framework.DrawCircle(instance, statusMessage, 300, 550, 150);
framework.DrawRectangle(instance, statusMessage, 500, 300, 400, 10);

reply = Reply(status = True, message = statusMessage.value)
print(reply.toString())

framework.ExportImage(instance, statusMessage, cString(imageName));