#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uMaxDepth;
uniform float uDepthOffset;
uniform float uTimeParameter;

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;

bool isNan(float val)
{
  return (val <= 0.0 || 0.0 <= val) ? false : true;
}

void main() {
    float r = vFlowCoord.x + .5 *  pow(vFlowCoord.y, 1.5);

    r = (vFlowCoord.x + uDepthOffset) / uMaxDepth + .5 *  pow(vFlowCoord.y, 1.5);

    r = fract(5. * r - 1. * 4 * uTimeParameter);
    if(r < .8)
        r = r / .8;
    else
        r = 1. - (r - .8) / .2;
    gl_FragColor = vec4(.5 + .5 * r, 0., 0.5, 1);
}
