import socket, threading, time, errno, ctypes, tkinter as tk
from ctypes import *
import ImageWriterPackage as iw

# Winsock Constant: Socket operation on non-socket
WSAENOTSOCK = 10038

def runNetworkService():

    global connToClient
    global serverSocket

    framework = window.framework

    # set up listening socket (IPv4 + TCP)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket:

        # Prevent "Address already in use" errors upon restart
        serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        serverSocket.bind((iw.HOST_IP, iw.PORT))
        serverSocket.listen()

        # continually come back here to listen for new clients after previous ones disconnect
        while True:

            print(f"Server: listening on {iw.HOST_IP}:{iw.PORT}...\n")

            # listening state: wait here until a client connects
            if 'connToClient' in globals():
                del connToClient

            try:
                connToClient, clientAddr = serverSocket.accept()
            except OSError as e:

                # Check if the error is due to the socket being intentionally closed or invalidated
                # from another thread
                if e.errno in (errno.EBADF, errno.EINVAL) or getattr(e, 'winerror', None) == WSAENOTSOCK:
                    print("Listening socket was closed. exiting service loop.")
                    return
                else:
                    # This is a real, unexpected network or system error
                    raise e

            with connToClient:
                print(f"Server: Connected to a client at {clientAddr}")

                # continually listen for new messages from client
                while True:

                    # wait here and receive command from client
                    commandBuffer = bytearray()
                    status, errorMessage = iw.receiveMessage(buffer = commandBuffer, connection = connToClient, size = iw.COMMAND_SIZE)
                    if not status:
                        # print error message and return to listening state
                        print(errorMessage)
                        break

                    command = iw.Command(int.from_bytes(commandBuffer))

                    # wait here and receive message from client
                    paramsBuffer = bytearray()
                    
                    match command:
                        
                        case iw.Command.SetCameraTransform:
                            
                            status, errorMessage = iw.receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(iw.Transform))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize params
                            transform = iw.Transform.from_buffer_copy(paramsBuffer)
                            print(f"From client: {transform}")

                            reply = iw.PlainReply()
                            framework.SetCameraTransform(reply, transform)

                            if not sendReplyToApp(reply):
                                break;

                        case iw.Command.GetCameraTransform:

                            print("From client: Getting the camera transform")

                            reply = iw.TransformReply()
                            framework.GetCameraTransform(reply)

                            if not sendReplyToApp(reply):
                                break;

                        case iw.Command.AddEntity:

                            status, errorMessage = iw.receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(iw.EntityData))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize params
                            entityParams = iw.EntityData.from_buffer_copy(paramsBuffer)
                            print(f"From client: {entityParams}")

                            
                            reply = iw.AddEntityReply()
                            framework.AddEntity(reply, entityParams)

                            if not sendReplyToApp(reply):
                                break;

                        case iw.Command.ModifyEntity:

                            status, errorMessage = iw.receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(iw.EntityData))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break

                            # deserialize params
                            entityParams = iw.EntityData.from_buffer_copy(paramsBuffer)
                            print(f"Modifying entity with id {entityParams.entityId} From client: {entityParams}")

                            
                            reply = iw.PlainReply()
                            framework.ModifyEntity(reply, entityParams)

                            if not sendReplyToApp(reply):
                                break;
                            
                        case iw.Command.GetEntityData:
                            
                            status, errorMessage = iw.receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(ctypes.c_uint64))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            
                            entityId = ctypes.c_uint64.from_buffer_copy(paramsBuffer)
                            print(f"Getting data on entity with id {entityId.value}")

                            reply = iw.EntityDataReply()
                            framework.GetEntityData(reply, entityId)

                            if not sendReplyToApp(reply):
                                break;

                        case iw.Command.GetSceneSnapshot:
                            
                            # capture atomic snapshot of scene at this moment
                            frameworkReply = iw.PlainReply()
                            snapshot = iw.Snapshot()
                            framework.MakeSnapshot(frameworkReply, snapshot)

                            # if the snapshot was not successfully produced, do nothing else
                            # here and return to listening mode, client will have also received the
                            # failure and will likely issue a disconnect command
                            if not frameworkReply.success:
                                reply = iw.NumEntitiesReply(success = frameworkReply.success, message = frameworkReply.message, numEntities = 0)
                                if not sendReplyToApp(reply):
                                    break;
                                continue

                            # send num entities back to client (so it can define SceneSnapshotReply accordingly)
                            numShapshotEntities = ctypes.c_uint64()
                            framework.GetNumEntitiesFromSnapshot(frameworkReply, snapshot, numShapshotEntities)
                            reply = iw.NumEntitiesReply(success = frameworkReply.success, message = frameworkReply.message, numEntities = numShapshotEntities)

                            # return num entites to client
                            if not sendReplyToApp(reply):
                                break;

                            # if the number of entities was not successfully queried, do nothing else
                            # here and return to listening mode, client will have also received the
                            # failure and will likely issue a disconnect command
                            if not frameworkReply.success:
                                continue

                            # define SceneSnapshotReply given num entities
                            print(f"Calling GetSceneSnapshot with {reply.numEntities} entities")
                            class SceneSnapshotReply(ctypes.Structure):
                                _fields_ = [
                                    ("success", ctypes.c_bool),
                                    ("message", ctypes.c_char * iw.MAX_REPLY_MESSAGE_LENGTH),
                                    ("cameraTransform", iw.Transform),
                                    ("cameraWidthFromHeight", ctypes.c_float), # only used when getting camera transform
                                    ("entitiesData", iw.EntityData * reply.numEntities),
                                ]

                                def __str__(self):
                                    return f"(SceneSnapshotReply: success = {self.success}, message = {self.message.decode('utf-8')}, entitiesData = <Entity Data>)"

                            
                            # initialize snapshot reply
                            reply = SceneSnapshotReply(success = False, message = b"UNINITIALIZED_MESSAGE")

                            cameraWidthFromHeight = ctypes.c_float() # Note: declaring this here since passing in reply.cameraWidthFromHeight doesn't resolve to the underlying ctype

                            # make framework call to get full snapshot, (can't pass in SceneSnapshotReply so just pass in plain frameworkReply and copy status after)
                            framework.GetSnapshotData(frameworkReply, snapshot, reply.cameraTransform, cameraWidthFromHeight, numShapshotEntities, reply.entitiesData)
                            reply.success = frameworkReply.success
                            reply.message = frameworkReply.message

                            reply.cameraWidthFromHeight = cameraWidthFromHeight

                            # return snapshot to client
                            if not sendReplyToApp(reply):
                                break;

                            framework.DisposeSnapshot(frameworkReply, snapshot)

                        case iw.Command.RemoveEntity:
                            
                            status, errorMessage = iw.receiveMessage(buffer = paramsBuffer, connection = connToClient, size = ctypes.sizeof(ctypes.c_uint64))
                            if not status:
                                # print error message and return to listening state
                                print(errorMessage)
                                break
                            
                            entityId = ctypes.c_uint64.from_buffer_copy(paramsBuffer)
                            print(f"From client: Removing entity with id {entityId.value}")

                            reply = iw.PlainReply()
                            framework.RemoveEntity(reply, entityId)

                            if not sendReplyToApp(reply):
                                break;

                        case iw.Command.RemoveAllEntities:

                            print("From client: removing all entities from scene")

                            reply = iw.PlainReply()
                            framework.RemoveAllEntities(reply)

                            if not sendReplyToApp(reply):
                                break;

                        case iw.Command.Disconnecting:
                            
                            print(f"Client at {clientAddr} gracefully disconnected\n\n")

                            # break out of client listening loop and go back to listening for
                            # any new clients listening on port
                            connToClient.close()
                            break
                            
                        case _:
                            print("Unrecognized command")
                            reply = iw.PlainReply()
                            reply.success = False,
                            reply.message = ("Unrecognized command sent to ImageWriterApp over socket").encode('utf-8')

                            if not sendReplyToApp(reply):
                                break;
                            


                    


def sendReplyToApp(reply) -> bool:
    # serialize client message
    replyBuffer = bytes(reply)

    # send reply to client
    sendStatus, sendResultMessage = iw.sendMessage(buffer = replyBuffer, connection = connToClient)

    if not sendStatus:
        # print error message and return to listening state
        print("Error sending message to client: " + sendResultMessage)
        return False

    print(f"To client: {reply}")
    return True

# main
if __name__ == "__main__":

    #original_val = ctypes.c_uint64(352)
    #buffer = bytes(original_val)
    #returnedVal = ctypes.c_uint64.from_buffer_copy(buffer)
    #print(original_val)
    #print(returnedVal)
    #exit(0)

    # create UI
    window = iw.Window(name = "ImageWriter App", windowSize="1920x1480")   

    serviceThread = threading.Thread(target = runNetworkService)
    serviceThread.start()

    # run UI
    window.onFrameUpdate()
    window.mainloop()
    
    # close sockets
    try:
        connToClient
    except NameError:
        print("Disposed cleanly - No client socket active")
    else:
        print("Warning: Disposing app while client socket was active")
        connToClient.close()
    serverSocket.close()

    serviceThread.join()

    # dispose
    window.cleanUp()
