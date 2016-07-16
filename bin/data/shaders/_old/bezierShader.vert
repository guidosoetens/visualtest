#version 130

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;

void main() {
    gl_Position = ftransform();
    
	vPosition = gl_Vertex.xy;
	vNormal = gl_Normal;
	vTexCoord = gl_MultiTexCoord0.xy;
}
