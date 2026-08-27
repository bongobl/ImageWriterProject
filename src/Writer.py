import ctypes
from ctypes import *

framework = ctypes.CDLL("./ImageWriterAPI.dll")

framework.GenImage.argtypes = None
framework.GenImage.restype = None

framework.GenImage()