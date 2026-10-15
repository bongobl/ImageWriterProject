//#version 460

uniform mat4 homogFromWorld;
uniform mat4 worldFromModel;
uniform vec2 scaleOffset;
uniform vec4 color;

void main()
{
    vec4 offsetScaledVertex = gl_Vertex;
    offsetScaledVertex.x *= scaleOffset.x;
    offsetScaledVertex.y *= scaleOffset.y;

    // transform the vertex position
    gl_Position =  homogFromWorld * worldFromModel * offsetScaledVertex;

    // forward the vertex color
    gl_FrontColor = color;
}
