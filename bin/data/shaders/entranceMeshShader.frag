#version 130

precision mediump float;

uniform int uShadowMode;
uniform float uRotation;

varying vec2 vScenePosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

vec2 rotate2D(vec2 vec, float angle) {
    float cs = cos(angle);
    float sn = sin(angle);
    return vec2(cs * vec.x - sn * vec.y, sn * vec.x + cs * vec.y);
}

void main(void) {

    if(uShadowMode == 1) {
        gl_FragColor = vec4(.15,.1,.2,1);
        return;
    }

    vec3 normal = normalize(vNormal);
    //normal.z = pow(normal.z, 0.2);
    //normal = normalize(vNormal);
    //normal.xy = rotate2D(normal.xy, uRotation);
    gl_FragColor = vec4(.5 + .5 * normal, 1.0);


}