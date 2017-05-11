#version 130

precision mediump float;

// Uniforms
uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform vec4 uBaseColor;
uniform sampler2D uSpotTexture;

uniform vec3 uDarkColor;
uniform vec3 uLightColor;
uniform vec3 uHighlightColor;

// Varying
varying vec2 vScenePosition;
varying vec3 vNormal;
varying float vOffsetFactor;

const float HIGHLIGHT_HUE = 0.77;
const float BASE_HUE = 0.8;
const float SHADOW_HUE = 0.83;

#define uCellTexture uTexture

const vec3 uLightPosition = vec3(.5, -.5, 1.0);
const vec3 uLightVector = normalize(vec3(1,-1,1));

// Varying
#define scenePosition vScenePosition
#define normal vNormal
//varying float effect;

const float pi = 3.14159265359;
const float cellSamples = 20.;
const float sqrtHalf = 0.70710678118;
const float sqrtThird = 0.57735026919;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void) {
    gl_FragColor = vec4(.5 + .5 * vNormal,1);

    vec3 dir = normalize(vec3(1,-1,0.4));
    dir = vec3(0,0,1);

    vec3 normal = vNormal;
    normal.z = pow(normal.z, 0.5);
    normal = normalize(normal);
    float offset = .5 + .5 * dot(dir, normal);
    normal.z += .5 * (.5 + .5 * cos(pow(fract(9 * (offset + uTime)), .5) * 2 * pi));
    normal = normalize(normal);

    vec3 lightdir = normalize(vec3(1,-1,2.5));
    float b = .5 + .5 * dot(lightdir, normal);

    gl_FragColor = vec4(b, .5 * b, 0., 1.);


    float highlightThreshold = 0.95;
    if(b < highlightThreshold) {
        gl_FragColor.rgb = mix(uDarkColor, uLightColor, pow(b / highlightThreshold, 1));
    }
    else {
        gl_FragColor.rgb = mix(uLightColor, uHighlightColor, (b - highlightThreshold) / (1 - highlightThreshold));
    }

    if(vOffsetFactor < 0)
        gl_FragColor.rgb = mix(gl_FragColor.rgb, uDarkColor, .5);

    // gl_FragColor.rgb = mix(uDarkColor, uLightColor, b);
    // if(b > .95)
    //     gl_FragColor.rgb = mix(gl_FragColor.rgb, uHighlightColor, pow((b - .95) / .05, 2.));

    // if(vOffsetFactor < 0)
    //     gl_FragColor.rgb = mix(gl_FragColor.rgb, uDarkColor, .5);

    /*
        uniform vec3 uDarkColor;
        uniform vec3 uLightColor;
        uniform vec3 uHighlightColor;
    */


    /*
    varying vec2 vScenePosition;
varying vec3 vNormal;
varying float vOffsetFactor;
    */
}