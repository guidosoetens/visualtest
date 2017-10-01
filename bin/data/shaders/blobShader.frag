#version 130

precision mediump float;

uniform int uShadowMode;
uniform float uRotation;

uniform float uStretch;
uniform float uHeadDistance;
uniform float uHeadRadius;
uniform float uAnimParam;

uniform vec3 uColor1;
uniform vec3 uColor2;
uniform vec3 uColor3;

#define darkColor uColor1
#define midColor uColor2
#define lightColor uColor3
/*
    // vec3 darkColor = vec3(.0, .2, .3);
    // vec3 midColor = vec3(.05, .5, .3);
    // vec3 lightColor = vec3(0.1, 1., .4);
*/

varying vec2 vModelPosition;
varying vec2 vScenePosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

const float pi = 3.14159;

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
    normal.z = pow(normal.z, 2.0);
    normal = normalize(normal);

    vec3 lightdir = normalize(vec3(300,-300,1500) - vec3(vScenePosition, 20 * normal.z));
    lightdir = normalize(vec3(500,-500,500) - vec3(vScenePosition, 0 * normal.z));
    float b = dot(lightdir, normal);
    b = pow(b, 2.0);

    vec4 surfaceColor = vec4(1.);

    float alpha = 1. - .2 * uStretch - .5 * pow(normal.z, 2.);
    // vec3 darkColor = vec3(.0, .2, .3);
    // vec3 midColor = vec3(.05, .5, .3);
    // vec3 lightColor = vec3(0.1, 1., .4);
    if(b < .5)
        surfaceColor.rgb = mix(darkColor, midColor, b / .5);
    else
        surfaceColor.rgb = mix(midColor, lightColor, (b - .5) / .5);

    surfaceColor.a = alpha * (.5 + .5 * b);

    gl_FragColor = vec4(midColor, .9 - .0 * uStretch);

    gl_FragColor.rgb = mix(gl_FragColor.rgb, surfaceColor.rgb, surfaceColor.a);

    float baseThreshold = .5 - .2 * clamp((uHeadDistance - vModelPosition.x) / 40.0, 0, 1);

    float threshold = baseThreshold + .1 * sin(4 * uAnimParam * 2 * pi + 20. * vTexCoord.x);
    float y = pow(vTexCoord.y, 2.0);
    if(y > threshold) {

        gl_FragColor.rgb = mix(gl_FragColor.rgb, darkColor, .4 * clamp((y - threshold) / .1, 0, 1));
    }

    threshold = (baseThreshold + .1) + .025 * sin(2 * uAnimParam * 2 * pi + 36. * vTexCoord.x);
    if(y > threshold) {
        gl_FragColor.rgb = mix(gl_FragColor.rgb, .9 * darkColor, .6 * clamp((y - threshold) / .3, 0, 1));
    }

    if(b > .9) {
        float highlight = pow((b - .9) / .1, 3.0);
        gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(1), .5 * highlight);
        //alpha = mix(alpha, 1.0, highlight);
    }

    //gl_FragColor = mix(innerColor, surfaceColor, surfaceColor.a);



}