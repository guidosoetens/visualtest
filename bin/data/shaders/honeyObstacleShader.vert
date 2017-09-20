#version 130

varying vec3 vScenePosition;
varying vec2 vModelPosition;
varying vec3 vNormal;
varying float vOffsetFactor;

void main(void)
{
    gl_Position = ftransform();

    vScenePosition = gl_Position.xyz;
    vScenePosition.z = 0;
	vModelPosition = gl_Vertex.xy;
	vOffsetFactor = gl_MultiTexCoord0.x;
    vNormal = gl_Normal.xyz;
}