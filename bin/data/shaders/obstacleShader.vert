#version 130

varying vec2 vScenePosition;
varying vec3 vNormal;
varying float vOffsetFactor;

void main(void)
{
    gl_Position = ftransform();

	vScenePosition = gl_Vertex.xy;
	vOffsetFactor = gl_MultiTexCoord0.x;
    vNormal = gl_Normal.xyz;
}