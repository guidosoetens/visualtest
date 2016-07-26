#version 130

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

uniform float uBoundOffset;

uniform float uDepthOffset;
uniform float uMaxDepth;

uniform int uDrawMode;
uniform float uRevealParameter;
uniform int uDeformNode;
uniform vec2 uSurfaceNormal;

uniform float uNodeRadius;
uniform vec2 uNodeCenter;
uniform int uNodeIsExternal;

void main() {

	vec2 flowCoord = gl_MultiTexCoord0.xy;

	float offset = uBoundOffset;
	vec4 vertex = gl_Vertex;



/*
	if(uDeformNode == 1) {
		if(abs(flowCoord.x) < .2) {
			float effect = 1.0 - abs(flowCoord.x) / .2;
			effect *= .8;
			vertex.xy = effect * uNodeCenter + (1. - effect) * vertex.xy;
		}
	}*/



	if(uDrawMode == 0) {

		//offset += 10. * uRevealParameter;

		/*
		float transitionOffset = .5;

		//uRevealParameter

		float relOffset = (uDepthOffset + flowCoord.x) / (uMaxDepth * (1. + transitionOffset));

		if(relOffset < uRevealParameter - transitionOffset) {
			//do nothing...
		}
		else if(relOffset < uRevealParameter) {
			//gradient:
			float t = (uRevealParameter - relOffset) / transitionOffset;
			vertex.z *= t;
		}
		else {
			vertex.z = 0;
		}
		*/
	}

    //gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (vertex + offset * vec4(gl_Normal.xy, 0, 0));
	vPosition = vertex.xyz;
	vNormal = gl_Normal;
	vFlowCoord = flowCoord;
}
