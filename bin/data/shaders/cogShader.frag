#version 130

precision mediump float;

// Varying
varying vec2 vScenePosition;
varying vec3 vNormal;

const float pi = 3.14159265359;
const float sqrtHalf = 0.70710678118;
const float sqrtThird = 0.57735026919;

void main(void) {
    gl_FragColor = vec4(1,0,0,1);//vec4(.5 + .5 * vNormal,1);
}