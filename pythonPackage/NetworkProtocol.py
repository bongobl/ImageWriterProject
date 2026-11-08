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
    ModifyEntity = 4
    GetEntityData = 5
    GetAllEntityData = 6
    RemoveEntity = 7
    RemoveAllEntities = 8
    Disconnecting = 9

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

def sendMessage(buffer: bytes, connection: socket.socket) -> tuple[bool, str]:

    # send reply to client
    try:
        connection.sendall(buffer)
    except ConnectionResetError as e:
        return (False, f"sendMessage: {e}")

    return (True, "")