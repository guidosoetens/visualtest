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

    vec3 lightdir = normalize(vec3(300,-300,1500) - vec3(vScenePosition, 20 * normal.z));
    lightdir = normalize(vec3(500,-500,1000) - vec3(vScenePosition, 0 * normal.z));
    float b = dot(lightdir, normal);
    b = pow(b, 2.0);

    vec3 darkColor = vec3(.2, .0, .3);
    vec3 midColor = vec3(.5, .05, .3);
    vec3 lightColor = vec3(1.0, .1, .4);

    darkColor = vec3(.2, .3, .6);
    midColor = vec3(.5, .7, .65);
    lightColor = vec3(.6, 1., .9);

    if(b < .5)
        gl_FragColor.rgb = mix(darkColor, midColor, b / .5);
    else
        gl_FragColor.rgb = mix(midColor, lightColor, (b - .5) / .5);

    if(b > .9) {
        float highlight = pow((b - .9) / .1, 4.0);
        gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(1), highlight);
    }
}