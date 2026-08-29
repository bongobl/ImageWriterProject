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

                # read message from console
                message = input("Enter message for server: ")

                # send message to server
                try:
                    clientSocket.sendall(message.encode('utf-8'))

                # will fail if server had disconnected at time of sending message
                # return to connecting state
                except ConnectionResetError as e:
                    logger.info(f"ConnectionResetError: {e}\n\n")
                    break

                # if we sent "Close", terminate client app
                if message == "Close":
                    return

                # receive reply from server and print it
                try:
                    replyFromServer = clientSocket.recv(MAX_RECV_BUFFER_SIZE)

                # will fail if server disconnects while we were waiting to receive its reply
                # return to connecting state
                except ConnectionResetError as e:
                    logger.info(f"ConnectionResetError: {e}\n\n")
                    break
                
                # error check message, failure likely means pipe is broken
                # return to connecting state
                if not replyFromServer:
                    logger.info("Error: failed to receive message from server\n\n")
                    break
                
                logger.info(f"Received reply: {replyFromServer.decode('utf-8')}")

if __name__ == "__main__":
    runClient()