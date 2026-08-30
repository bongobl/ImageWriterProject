import ctypes

HOST_IP = '127.0.0.1'
PORT = 65432
MAX_RECV_BUFFER_SIZE = 1024
MAX_PLAYER_NAME_LENGTH = 32

# Message to server
class ServerMessage(ctypes.Structure):
    _fields_ = [
        ("playerName", ctypes.c_char * MAX_PLAYER_NAME_LENGTH),
        ("health", ctypes.c_float),
        ("powerLevel", ctypes.c_int),
        ("age", ctypes.c_int),
        ("strength", ctypes.c_float),
    ]

    def toString(self):
        return f"ServerMessage:playerName = {self.playerName.decode('utf-8')}, health = {self.health}, powerLevel = {self.powerLevel}, age = {self.age}, strength = {self.strength}"

SERVER_MESSAGE_SIZE = ctypes.sizeof(ServerMessage)

# Message to client
class ClientMessage(ctypes.Structure):
    _fields_ = [
        ("length", ctypes.c_float),
        ("width", ctypes.c_float),
        ("height", ctypes.c_float),
        ("resources", ctypes.c_int),
    ]

    def toString(self):
        return f"ClientMessage:length = {self.length}, width = {self.width}, height = {self.height}, resources = {self.resources}"

CLIENT_MESSAGE_SIZE = ctypes.sizeof(ClientMessage)