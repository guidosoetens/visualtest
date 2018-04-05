#version 130

precision mediump float;

uniform sampler2DRect uTexImage;
uniform float uTexDimension;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

varying float vBaseAlpha;
varying vec2 vLineTexCoord;

void main() {
    gl_FragColor = vec4(1,1,1, (vBaseAlpha + texture2DRect(uTexImage, vLineTexCoord * uTexDimension).r) * pow(1. - vFlowCoord.y, .5) * 0.3);
}