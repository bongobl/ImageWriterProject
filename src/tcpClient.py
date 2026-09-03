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

                # read in command
                while True:
                    commandInt = int(input("What would you like to do?\n" \
                    " - (1) = to setup image\n" \
                    " - (2) = to draw a circle\n" \
                    " - (3) = draw a rectangle\n" \
                    " - (4) = export image\n" \
                    " - (5) = disconnect from server\n" \
                    "Choice: "))

                    if commandInt >= 1 and commandInt <= len(Command):
                        commIn = Command(commandInt)
                        commandBuffer = bytes(commIn.value.to_bytes(COMMAND_SIZE))
                        break
                    logger.error(f"select a number between 1 and {len(Command)}\n")

                # read in and serialize params buffer
                hasParams = True
                paramsBuffer = bytearray()
                match commIn:

                    case Command.SetupImage:

                        # obtain setupImage params
                        while True:
                            inputArgs = input("Enter the width and height of the image: ").split()
                            if len(inputArgs) == 2:
                                break;
                            logger.error(" -- Two (and only two) integers are expected -- ")
        
                        width, height = tuple(int(x) for x in inputArgs)
        
                        # create drawCircle command params
                        setupImageParams = SetupImageParams(width = width, height = height)
                        logger.info(f"To server: {setupImageParams.toString()}")

                        # serialize params
                        paramsBuffer = bytes(setupImageParams)
                    case Command.DrawCircle:

                        # obtain drawCircle params
                        while True:
                            inputArgs = input("Enter the centerX, centerY and radius of the circle: ").split()
                            if len(inputArgs) == 3:
                                break;
                            logger.error(" -- Three (and only three) integers are expected -- ")
        
                        centerX, centerY, radius = tuple(int(x) for x in inputArgs)
        
                        # create drawCircle command params
                        circleParams = DrawCircleParams(centerX = centerX, centerY = centerY, radius = radius)
                        logger.info(f"To server: {circleParams.toString()}")

                        # serialize params
                        paramsBuffer = bytes(circleParams)

                    case Command.DrawRectangle:

                        # obtain drawRectangle params
                        while True:
                            inputArgs = input("Enter the centerX, centerY, halfExtentX and halfExtentY of the rectangle: ").split()
                            if len(inputArgs) == 4:
                                break;
                            logger.error(" -- Four (and only four) integers are expected -- ")
        
                        centerX, centerY, halfExtentX, halfExtentY = tuple(int(x) for x in inputArgs)
        
                        # create drawRectangle command params
                        rectangleParams = DrawRectangleParams(centerX = centerX, centerY = centerY, halfExtentX = halfExtentX, halfExtentY = halfExtentY)
                        logger.info(f"To server: {rectangleParams.toString()}")

                        # serialize params
                        paramsBuffer = bytes(rectangleParams)

                    case Command.ExportImage:

                        # obtain drawRectangle params
                        while True:
                            inputArgs = input("Enter the filename of the image to export: ").split()
                            if len(inputArgs) == 1:
                                break;
                            logger.error(" -- One (and only one) string is expected -- ")

                        imageName = inputArgs[0]

                        # create exportImage command params
                        exportImageParams = ExportImageParams(imageName = imageName.encode('utf-8'))
                        logger.info(f"To server: {exportImageParams.toString()}")

                        # serialize params
                        paramsBuffer = bytes(exportImageParams)

                    case Command.Disconnecting:
                        logger.info(f"To server: Disconnecting")
                        hasParams = False

                # send message to server
                try:
                    connToServer.sendall(commandBuffer)

                    if hasParams:
                        connToServer.sendall(paramsBuffer)

                # will fail if server had disconnected at time of sending message
                # return to connecting state
                except ConnectionResetError as e:
                    logger.error(f"ConnectionResetError: {e}\n\n")
                    break

                # if we disconnected from server, don't wait for reply
                if commIn == Command.Disconnecting:
                    logger.info(f"Disconnected from server")
                    return

                replyBuffer = bytearray()

                # wait here and receive message from client
                status, errorMessage = receiveMessage(buffer = replyBuffer, connection = connToServer, size = ctypes.sizeof(DummyReply))
                if not status:
                    # print error message and return to connecting state
                    logger.error(errorMessage)
                    break
                
                # deserialize client message and print
                reply = DummyReply.from_buffer_copy(replyBuffer)
                logger.info(f"From server: {reply.toString()}")

if __name__ == "__main__":
    
    runClient()