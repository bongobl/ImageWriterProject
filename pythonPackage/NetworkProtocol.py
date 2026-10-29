import ctypes, socket
from pydantic import BaseModel
from enum import IntEnum
from .Common import *

HOST_IP = '127.0.0.1'
PORT = 65432
COMMAND_SIZE = 1

# Commands
class Command(IntEnum):

    SetCameraTransform = 1
    GetCameraTransform = 2
    AddEntity = 3
    GetEntityData = 4
    RemoveEntity = 5
    RemoveAllEntities = 6
    Disconnecting = 7

def receiveMessage(buffer: bytearray, connection: socket.socket, size: int) -> tuple[bool, str]:

    # read server message from client
    try:
        while len(buffer) < size:
            fragmentReceived = connection.recv(size - len(buffer))

            # error check on data received
            if not fragmentReceived:
                # sanity check buffer
                return (False, "receiveMessage: failed to receive data")
            
            buffer.extend(fragmentReceived)

    # will fail if client disconnects without gracefully telling us
    # return to listening state
    except (ConnectionResetError, ConnectionAbortedError) as e:
        return (False, f"receiveMessage: {e}")

    return (True, "")