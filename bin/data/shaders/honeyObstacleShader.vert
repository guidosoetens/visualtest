#version 130

varying vec2 vScenePosition;
varying vec2 vModelPosition;
varying vec3 vNormal;
varying float vOffsetFactor;

void main(void)
{
    gl_Position = ftransform();

    vec4 locVert = gl_ModelViewMatrix * gl_Vertex;

	vScenePosition = (locVert.xyz / locVert.w).xy;

    // vScenePosition = gl_Position.xyz;
    // vScenePosition.z = 0;
	vModelPosition = gl_Vertex.xy;
	vOffsetFactor = gl_MultiTexCoord0.x;
    vNormal = gl_Normal.xyz;
}