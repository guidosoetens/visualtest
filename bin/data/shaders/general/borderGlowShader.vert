#version 130

uniform float uBoundOffset;

uniform float uMaxDepth;
uniform float uRevealParameter;
uniform float uWinAnimParameter;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

varying float vBaseAlpha;
varying vec2 vLineTexCoord;

const float pi = 3.1415926535;

void main() {

	vec2 flowCoord = gl_MultiTexCoord0.xy;

	float offset = (flowCoord.y < .5) ? 0.0 : uBoundOffset;

	vec4 vertex = gl_Vertex;

	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (vertex + offset * vec4(gl_Normal.xy, 0, 0));
	vPosition = vertex.xyz;
	vNormal = gl_Normal;
	vFlowCoord = flowCoord;


	//calculate base alpha:
	float absBlendOffset = .3;
    float additionalOffset = 1.5 + absBlendOffset;
    float absDepth = (vFlowCoord.x + .5 * pow(max(0.0,vFlowCoord.y), 1.5));
    float relDepth = absDepth / (uMaxDepth + additionalOffset);
    float absRevealThreshold = uRevealParameter * (uMaxDepth + additionalOffset);
    float postWinFadeFactor = max(0.0, 1.0 - 4.0 * clamp(uWinAnimParameter, 0., 1.));
    float shineFactor = 0.0;
    if(relDepth < uRevealParameter)
        shineFactor = min(1.0, absRevealThreshold - absDepth);
    shineFactor *= postWinFadeFactor;
    vBaseAlpha = .5 * min(1.0, 2. * shineFactor * (1. - vFlowCoord.y));



	if(uWinAnimParameter > -1.0) {
		if(uWinAnimParameter < 0.0) {
			float t = .5 - .5 * cos((uWinAnimParameter + 1.0) * pi);
			float loc_t = fract(t * 3.0);
			vLineTexCoord.x = 0.0;
			vLineTexCoord.y = (5. - 3. * t) * ((1. - vFlowCoord.y) - loc_t);
			vBaseAlpha += vFlowCoord.y * 1. * t;
		}
		else {
			float t = sin(pow(uWinAnimParameter, .5) * .5 * pi);
			vLineTexCoord.x = 0.0;
			vLineTexCoord.y = (2. + 2. * t) * (vFlowCoord.y - t);
			vBaseAlpha += vFlowCoord.y * 1. * (1. - t);
		}
	}
	else {
		vLineTexCoord = vec2(-1);
	}
}