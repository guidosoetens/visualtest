#version 130

precision mediump float;

uniform int uShadowMode;

varying vec2 vTexCoord;
varying vec3 vNormal;


void main(void) {

    if(uShadowMode == 1) {
        gl_FragColor = vec4(.15,.1,.2,1);
        return;
    }

    vec3 normal = normalize(vNormal);
    gl_FragColor = vec4(.5 + .5 * normal, 1.0);
}