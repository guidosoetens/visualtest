#version 130

precision mediump float;

uniform vec2 uResolution;
varying vec2 vPosition;
varying vec3 vNormal;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);

void main() {

    //vec2 scale = vec2(1.0, uResolution.y / uResolution.x);
    //vec2 uv = (scale * vPosition / uResolution + 1.0) / 2.0;

    vec3 normal = normalize(vNormal);
    //normal.z = pow(normal.z, 0.5);
    //normal = normalize(normal);
    gl_FragColor = vec4(.5 + .5 * normal.xy, normal.z, 1);
}
