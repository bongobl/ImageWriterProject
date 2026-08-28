import ctypes
from ctypes import *

framework = ctypes.CDLL("./ImageWriterAPI.dll")

framework.GenImage.argtypes = [ctypes.c_int, ctypes.c_int]
framework.GenImage.restype = None

framework.GenImage(1280, 720)