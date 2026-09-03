import ctypes
from ctypes import *

framework = ctypes.CDLL("./ImageWriterAPI.dll")

# TODO: Mirror API.h: move to common header
class HImageWriterInstance(ctypes.Structure):
    _fields_ = [
        ("pData", ctypes.c_void_p),
    ]

framework = ctypes.CDLL("./ImageWriterAPI.dll")

framework.CreateImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
framework.CreateImageWriterInstance.restype = ctypes.c_bool

framework.SetupImage.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int]
framework.SetupImage.restype = ctypes.c_bool

framework.DrawCircle.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int, ctypes.c_int]
framework.DrawCircle.restype = ctypes.c_bool

framework.DrawRectangle.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
framework.DrawRectangle.restype = ctypes.c_bool

framework.ExportImage.argtypes = [HImageWriterInstance]
framework.ExportImage.restype = ctypes.c_bool

framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
framework.DestroyImageWriterInstance.restype = ctypes.c_bool

instance = HImageWriterInstance()


if not framework.CreateImageWriterInstance(ctypes.byref(instance)):
    print("Failed to create image writer instance")
    exit(1)

framework.SetupImage(instance, 1280, 720);
framework.DrawCircle(instance, 800, 300, 70);
framework.DrawCircle(instance, 300, 550, 150);
framework.DrawRectangle(instance, 500, 300, 400, 10);
framework.ExportImage(instance);