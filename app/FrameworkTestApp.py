import pygame, threading, time, ctypes, tkinter as tk
from ctypes import *
import ImageWriterPackage as iw

def createSomeSampleShapes():

    imageWriter = window.framework;

    input("Press Enter")

    transformReply = iw.TransformReply();
    if not imageWriter.GetCameraTransform(transformReply):
        print(f"ImageWriter error: {transformReply.message.decode('utf-8')}")
    else:
        print(f"ImageWriter success: {transformReply.message.decode('utf-8')}")
        print(f"camera scaleX = {transformReply.transform.scale * transformReply.cameraWidthFromHeight}, scaleY = {transformReply.transform.scale}")

    dimX = 8
    dimY = 6
    spacingX = 2.4
    spacingY = 1.3

    startX = (dimX - 1) / 2 * spacingX
    startY = (dimY - 1) / 2 * spacingY

    addEntityReply = iw.AddEntityReply()

    for i in range(dimY):
        for j in range(dimX):

            if not imageWriter.AddEntity(
                addEntityReply,
                iw.EntityData(
                    entityType = iw.EntityType.ELLIPSE, 
                    transform = iw.Transform(position = iw.Vec2(x = j * spacingX - startX, y = i * spacingY - startY), orientation = 0, scale = 1),
                    color = iw.Color(red = 1, green = 0, blue = 0, alpha = 1),
                    zOrder = -1,
                    ellipse = iw.EllipseData(halfExtents = iw.Vec2(0.5, 0.35))
                )
            ):
                print(f"ImageWriter error: {addEntityReply.message.decode('utf-8')}")
    
    plainReply = iw.PlainReply();
    entityDataReply = iw.EntityDataReply()
    input("Press Enter")
    if imageWriter.GetEntityData(entityDataReply, 26):
        entity26Pos = entityDataReply.entityData.transform.position;
        print(f"Found entity: pos = ({entity26Pos.x}, {entity26Pos.y})")
    else:
        print(f"ImageWriter error: {entityDataReply.message.decode('utf-8')}")

    if not imageWriter.RemoveEntity(plainReply, 26):
        print(f"ImageWriter error: {plainReply.message.decode('utf-8')}")

    if not imageWriter.GetEntityData(entityDataReply, 26):
        print(f"Corrent not-found entity: {entityDataReply.message.decode('utf-8')}")

    input("Press Enter")
    if not imageWriter.RemoveAllEntities(plainReply):
        print(f"ImageWriter error: {plainReply.message.decode('utf-8')}")

    input("Press Enter")
    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddEntity(
                addEntityReply,
                iw.EntityData(
                    entityType = iw.EntityType.RECTANGLE, 
                    transform = iw.Transform(position = iw.Vec2(x = j * spacingX - startX, y = i * spacingY - startY), orientation = 0, scale = 1),
                    color = iw.Color(red = 0, green = 1, blue = 1, alpha = 1),
                    zOrder = -1,
                    rectangle = iw.RectangleData(halfExtents = iw.Vec2(0.5, 0.35))
                )
            ):
                print(f"ImageWriter error: {addEntityReply.message.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(plainReply):
        print(f"ImageWriter error: {plainReply.message.decode('utf-8')}")

    input("Press Enter")
    for i in range(dimY):
        for j in range(dimX):
            if not imageWriter.AddEntity(
                addEntityReply,
                iw.EntityData(
                    entityType = iw.EntityType.TRIANGLE, 
                    transform = iw.Transform(position = iw.Vec2(x = j * spacingX - startX, y = i * spacingY - startY), orientation = 0, scale = 1),
                    color = iw.Color(red = 1, green = 1, blue = 0, alpha = 1),
                    zOrder = -1,
                    triangle = iw.TriangleData(
                        point1 = iw.Vec2(-0.5, 0.35), 
                        point2 = iw.Vec2(-0.5, -0.35), 
                        point3 = iw.Vec2(0.5, 0)
                    )
                )
            ):
                print(f"ImageWriter error: {addEntityReply.message.decode('utf-8')}")
    
    input("Press Enter")
    if not imageWriter.RemoveAllEntities(plainReply):
        print(f"ImageWriter error: {plainReply.message.decode('utf-8')}")



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