import tcpCommon, socket
from tcpCommon import *
import logging



logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imageWriterClient")

def runClient():

    while True:

        # configure socket (similarly to server socket)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as clientSocket:

            logger.info(f"Connecting to server at {HOST_IP}:{PORT}...")

            # connecting state: continually loop trying to connect to server
            while True:
                try:
                    clientSocket.connect((HOST_IP, PORT))
                    break
                except (TimeoutError, ConnectionRefusedError) as e:
                    logger.info(f"Connection attempt failed: {e}\nTrying again...")

            # established connection, enter message loop with server
            logger.info(f"Connection successful!")
            while True:

                # read player name from console
                playerName = input("Enter a name for the player: ")

                # create dummy server message
                serverMessage = ServerMessage(playerName = playerName.encode('utf-8'), health = 3.5, powerLevel = 8, age = 27, strength = 34.75)
                logger.info(f"In: {serverMessage.toString()}")

                # serialize server message
                serverMessageBuffer = bytes(serverMessage)

                # send message to server
                try:
                    clientSocket.sendall(serverMessageBuffer)

                # will fail if server had disconnected at time of sending message
                # return to connecting state
                except ConnectionResetError as e:
                    logger.info(f"ConnectionResetError: {e}\n\n")
                    break

                # if we sent "Close", terminate client app
                if playerName == "Close":
                    return


                bufferValid = True
                clientMessageBuffer = bytearray()

                # read client message from server
                try:
                    while len(clientMessageBuffer) < CLIENT_MESSAGE_SIZE:
                        fragmentReceived = clientSocket.recv(CLIENT_MESSAGE_SIZE - len(clientMessageBuffer))

                        # error check on data received
                        if not fragmentReceived:
                            logger.info("Client error: failed to receive data from server\n\n")
                            bufferValid = False
                            break

                        clientMessageBuffer.extend(fragmentReceived)

                    # sanity check buffer
                    # if not valid, return to connecting state
                    if not bufferValid:
                        break
                    
                # will fail if server disconnects while we were waiting to receive its reply
                # return to connecting state
                except (ConnectionResetError, ConnectionAbortedError) as e:
                    logger.info(f"recv error: {e}\n\n")
                    break
                
                # deserialize client message and print
                clientMessage = ClientMessage.from_buffer_copy(clientMessageBuffer)
                logger.info(f"Out: {clientMessage.toString()}")

if __name__ == "__main__":
    runClient()