#version 130

precision mediump float;

uniform float uRotation;
uniform sampler2D uBumpMap;
uniform float uRadius;

// Varying
varying vec2 vModelPosition;
varying vec2 vScenePosition;
varying vec3 vNormal;

const float pi = 3.14159265359;
const float sqrtHalf = 0.70710678118;
const float sqrtThird = 0.57735026919;

vec2 rotate2D(vec2 vec, float angle) {
    float cs = cos(angle);
    float sn = sin(angle);
    return vec2(cs * vec.x - sn * vec.y, sn * vec.x + cs * vec.y);
}

void main(void) {

    vec3 normal = vNormal;
    if(normal.z < 1.0) {
        //edge:
        normal = normalize(normal);
    }
    else {
        vec2 uv = fract(vModelPosition / 200.0);
        //uv.x = 1 - uv.x;
        //uv.y = 1 - uv.y;
        vec4 bumpColor = texture2D(uBumpMap, uv);
        normal = 2 * bumpColor.rgb - 1;
        normal.xy = rotate2D(normal.xy, uRotation);
        normal.z *= 5.0;
        normal = normalize(normal);
    }

    gl_FragColor = vec4(.5 + .5 * normal, 1);

    vec3 lightDir = normalize(vec3(1,-1,1));
    float b = .5 + .5 * dot(lightDir, normal);

    if(b < .5)
        gl_FragColor.rgb = mix(vec3(.7, .2, 1), vec3(1, .7, .7), b / .5);
    else
        gl_FragColor.rgb = mix(vec3(1, .7, .7), vec3(1, 1, 1), (b - .5) / .5);

    gl_FragColor.a = 1;// = vec4(b,b,b,1);
}