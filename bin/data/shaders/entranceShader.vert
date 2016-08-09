#version 130

varying vec2 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec4 vColor;

void main() {
    gl_Position = ftransform();
	vPosition = gl_Vertex.xy;
	vTexCoord = gl_MultiTexCoord0.xy;
    vNormal = gl_Normal.xyz;
    vColor = gl_Color;
}
