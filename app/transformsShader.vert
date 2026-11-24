
uniform mat4 homogFromWorld;
uniform float cameraScale;

void main()
{
    
    float axisLength = cameraScale / 10;
    float orientation = gl_MultiTexCoord0.x;
    int idInGroup = gl_VertexID % 4;
    vec2 offsetForThisId = vec2(0,0);

    if(idInGroup == 0){
        offsetForThisId = vec2(axisLength * cos(orientation),axisLength * sin(orientation));
    }else if(idInGroup == 3) {
        offsetForThisId = vec2(-axisLength * sin(orientation), axisLength * cos(orientation));
    }

    vec4 finalPos = gl_Vertex;
    finalPos.x = finalPos.x + offsetForThisId.x;
    finalPos.y = finalPos.y + offsetForThisId.y;

    // transform the vertex position
    gl_Position = homogFromWorld * finalPos;

    // forward the vertex color
    gl_FrontColor = idInGroup >= 2 ? vec4(0,1,0,1) : vec4(1,0,0,1);
}