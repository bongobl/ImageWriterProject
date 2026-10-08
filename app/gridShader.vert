#version 460

uniform mat4 homogFromCamera;
uniform mat4 cameraFromWorld;
uniform float cameraScale;

uniform float minX;
uniform float maxX;
uniform float minY;
uniform float maxY;
uniform int numXLines;
uniform int numTotalVerts;

float getColorScaleFactor(float size);
void main()
{

    vec4 gridPosition = vec4(0,0,0,1);

    float opacity = 1;
	if (gl_VertexID < numXLines * 2)
	{
		int ind = gl_VertexID;
		int x = int(ceil(minX) + ind / 2);
		int y = int(ind % 2 == 0 ? floor(minY) : ceil(maxY));
		gridPosition = vec4(x, y, 0, 1);
		opacity = x != 0 ? (x % 10 != 0 ? 0.2 * getColorScaleFactor(cameraScale) : 0.4) : 0.8;
	}
	else 
	{
		int ind = gl_VertexID - (numXLines * 2);
		int y = int(ceil(minY) + ind / 2);
		int x = int(ind % 2 == 0 ? floor(minX) : ceil(maxX));
		gridPosition = vec4(x, y, 0, 1);
		opacity = y != 0 ? (y % 10 != 0 ? 0.2 * getColorScaleFactor(cameraScale) : 0.4) : 0.8;
	}

    // transform the vertex position
    gl_Position =  homogFromCamera * cameraFromWorld * gridPosition;


    // forward the vertex color
    gl_FrontColor = vec4(0,opacity,opacity,1);
}

float getColorScaleFactor(float size){
	if(size <= 30){
		return 1.0f;
	}

	return (100 - size) / (100 - 30);
}