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
framework.CreateImageWriterInstance.restype = None

framework.SetupImage.argtypes = [HImageWriterInstance, ctypes.c_int, ctypes.c_int]
framework.SetupImage.restype = None

framework.ExportImage.argtypes = [HImageWriterInstance]
framework.ExportImage.restype = None

framework.DestroyImageWriterInstance.argtypes = [ctypes.POINTER(HImageWriterInstance)]
framework.DestroyImageWriterInstance.restype = None

instance = HImageWriterInstance()


framework.CreateImageWriterInstance(ctypes.byref(instance))
framework.SetupImage(instance, 1280, 720)
framework.ExportImage(instance)
framework.DestroyImageWriterInstance(instance)