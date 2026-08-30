import tcpCommon, socket
from tcpCommon import *
import logging



logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("imageWriterClient")

def runClient():

    while True:

        # configure socket (similarly to server socket)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as connToServer:

            logger.info(f"Connecting to server at {HOST_IP}:{PORT}...")

            # connecting state: continually loop trying to connect to server
            while True:
                try:
                    connToServer.connect((HOST_IP, PORT))
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
                    connToServer.sendall(serverMessageBuffer)

                # will fail if server had disconnected at time of sending message
                # return to connecting state
                except ConnectionResetError as e:
                    logger.info(f"ConnectionResetError: {e}\n\n")
                    break

                # if we sent "Close", terminate client app
                if playerName == "Close":
                    return

                clientMessageBuffer = bytearray()

                # wait here and receive message from client
                status, errorMessage = receiveMessage(buffer = clientMessageBuffer, connection = connToServer, size = CLIENT_MESSAGE_SIZE)
                if not status:
                    # print error message and return to connecting state
                    logger.info(errorMessage)
                    break
                
                # deserialize client message and print
                clientMessage = ClientMessage.from_buffer_copy(clientMessageBuffer)
                logger.info(f"Out: {clientMessage.toString()}")

if __name__ == "__main__":
    runClient()