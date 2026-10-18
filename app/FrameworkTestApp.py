import pygame, threading, time, ctypes, tkinter as tk
from ctypes import *
import ImageWriterPackage as iw

def createSomeSampleShapes():

    imageWriter = window.framework;
    statusMessage = ctypes.create_string_buffer(b"Command executed successfully", 256)

    input("Press Enter")

    cameraTransform = iw.Transform()
    widthFromheight = ctypes.c_float(0.0)
    if not imageWriter.GetCameraTransform(statusMessage, cameraTransform, widthFromheight):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    else:
        print(f"camera scaleX = {cameraTransform.scale * widthFromheight.value}, scaleY = {cameraTransform.scale}")

    dimX = 8
    dimY = 6
    spacingX = 2.4
    spacingY = 1.3

    startX = (dimX - 1) / 2 * spacingX
    startY = (dimY - 1) / 2 * spacingY

    for i in range(dimY):
        for j in range(dimX):

            if not imageWriter.AddEntity(
                statusMessage,
                iw.AddEntityParams(
                    entityType = iw.EntityType.ELLIPSE, 
                    transform = iw.Transform(position = iw.Vec2(x = j * spacingX - startX, y = i * spacingY - startY), orientation = 0, scale = 1),
                    color = iw.Color(red = 1, green = 0, blue = 0, alpha = 1),
                    ellipse = iw.AddEllipseParams(halfExtents = iw.Vec2(0.5, 0.35))
                )
            ):
                print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveEntity(statusMessage, 28):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    input("Press Enter")
    if not imageWriter.RemoveAllEntities(statusMessage):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    input("Press Enter")
    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddEntity(
                statusMessage,
                iw.AddEntityParams(
                    entityType = iw.EntityType.RECTANGLE, 
                    transform = iw.Transform(position = iw.Vec2(x = j * spacingX - startX, y = i * spacingY - startY), orientation = 0, scale = 1),
                    color = iw.Color(red = 0, green = 1, blue = 1, alpha = 1),
                    rectangle = iw.AddRectangleParams(halfExtents = iw.Vec2(0.5, 0.35))
                )
            ):
                print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(statusMessage):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    input("Press Enter")
    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddEntity(
                statusMessage,
                iw.AddEntityParams(
                    entityType = iw.EntityType.TRIANGLE, 
                    transform = iw.Transform(position = iw.Vec2(x = j * spacingX - startX, y = i * spacingY - startY), orientation = 0, scale = 1),
                    color = iw.Color(red = 1, green = 1, blue = 0, alpha = 1),
                    triangle = iw.AddTriangleParams(
                        point1 = iw.Vec2(-0.5, 0.35), 
                        point2 = iw.Vec2(-0.5, -0.35), 
                        point3 = iw.Vec2(0.5, 0)
                    )
                )
            ):
                print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(statusMessage):
        print(f"ImageWriter error: {statusMessage.value.decode('utf-8')}")

    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = cameraTransform.scaleX, positionY = cameraTransform.scaleY, scaleX = 1, scaleY = 1, angle = 0), 
    #     Color(red = 1, green = 0, blue = 0, alpha = 1)
    # )

    # instance.AddRectangle(statusMessage, 
    #     Transform(positionX = -cameraTransform.scaleX, positionY = -cameraTransform.scaleY, scaleX = 3, scaleY = 2, angle = 20), 
    #     Color(red = 0.5, green = 0.5, blue = 1, alpha = 1)
    # )

    # instance.AddTriangle(statusMessage,
    # 	Transform(positionX = 0, positionY = 0, scaleX = 1, scaleY = 1, angle = 60),
    # 	Color(red = 0, green = 1, blue = 0, alpha = 1),
    #     Vec2(1,-1), Vec2( 0,2 ), Vec2(-1,-1)
    # )
    
    # input("Press Enter")
    # instance.RemoveAllEntities(statusMessage);

    # input("Press Enter")
    # instance.AddRectangle(statusMessage, 
    #     Transform(positionX = 0, positionY = 0, scaleX = cameraTransform.scaleX - 1, scaleY = cameraTransform.scaleY - 1, angle = -30), 
    #     Color(red = 0.5, green = 1, blue = 0.5, alpha = 0.35 )
    # )
    # instance.AddTriangle(statusMessage,
    # 	Transform(positionX = 0, positionY = 0, scaleX = 1, scaleY = 1, angle = 0),
    # 	Color(red = 1, green = 1, blue = 0, alpha = 1),
    #     Vec2(3,1), Vec2(1,3), Vec2(-2,0)
    # )
    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = 3, positionY = -2, scaleX = 3, scaleY = 2, angle = -10), 
    #     Color(red = 0, green = 0, blue = 1, alpha = 0.4)
    # )

    # input("Press Enter")
    # instance.RemoveAllEntities(statusMessage);

    # input("Press Enter")
    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = -5, positionY = 3, scaleX = 1, scaleY = 4, angle = -25), 
    #     Color(red = 0, green = 1, blue = 1, alpha = 0.6)
    # )

    # instance.AddEllipse(statusMessage, 
    #     Transform(positionX = 7, positionY = -4, scaleX = 0.5, scaleY = 1.5, angle = 60), 
    #     Color(red = 1, green = 1, blue = 0, alpha = 0.2)
    # )

    # instance.AddRectangle(statusMessage, 
    #     Transform(positionX = -8, positionY = -1, scaleX = 1, scaleY = 1, angle = 12), 
    #     Color(red = 1, green = 0.5, blue = 1, alpha = 0.6)
    # )


if __name__ == "__main__":

    ttt = iw.Transform()
    # create UI
    window = iw.Window(name = "My Test App", windowSize="1920x1480")    
    
    apiThread = threading.Thread(target = createSomeSampleShapes)
    apiThread.start()

    # run UI
    window.onFrameUpdate()
    window.mainloop()
 
    apiThread.join()

    window.cleanUp()