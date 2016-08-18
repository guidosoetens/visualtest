#version 130

varying vec2 vScenePosition;
varying vec2 vTexCoord;

void main(void)
{
    gl_Position = ftransform();

	vScenePosition = gl_Vertex.xy;
	vTexCoord = gl_MultiTexCoord0.xy;
}