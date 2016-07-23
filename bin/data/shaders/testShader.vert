#version 130

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

uniform float uBoundOffset;

uniform int uDrawMode;
uniform float uRevealParameter;

void main() {

	float offset = uBoundOffset;
	vec4 vertex = gl_Vertex;
	if(uDrawMode == 0) {
		offset = uBoundOffset + 30. * uRevealParameter;
		//vertex.z = 0.;
	}

    //gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (vertex + offset * vec4(gl_Normal.xy, 0, 0));
	vPosition = gl_Vertex.xy;
	vNormal = gl_Normal;
	vFlowCoord = gl_MultiTexCoord0.xy;
}
