//#version 460

uniform mat4 homogFromCamera;
uniform mat4 cameraFromWorld;
uniform mat4 worldFromModel;
uniform vec2 scaleOffset;
uniform vec4 color;

void main()
{
    vec4 myVec = gl_Vertex;
    myVec.x *= scaleOffset.x;
    myVec.y *= scaleOffset.y;
    // transform the vertex position
    gl_Position =  homogFromCamera * cameraFromWorld * worldFromModel * myVec;

    // forward the vertex color
    gl_FrontColor = color;
}
