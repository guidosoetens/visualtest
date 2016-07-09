#version 130

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

void main() {
    gl_Position = ftransform();
	vPosition = gl_Vertex.xy;
	//gl_Vertex
	vNormal = gl_Normal;
	
	vFlowCoord = gl_MultiTexCoord0.xy;
}
