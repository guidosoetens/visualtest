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

float sampleThreshold(float angle) {
    return .65 + .08 * cos(2 * angle + 12 * uTime) * sin(3 * angle + .35 - 18 * uTime) * sin(3 * angle + .83 - 24 * uTime);
}

void main(void) {
    vec3 normal = vNormal;
    //normal.z = pow(normal.z, 0.5);
    //normal = normalize(normal);

    float angle = atan(normal.x, normal.y);
    float dist = 1 - normal.z;

    float d = dot(vScenePosition, vec2(.707,.707));
    float gray = .3;
    if(mod(d * .07, 1.0) < .5)
        gray = .15;
    gl_FragColor = vec4(gray, gray, gray, gray);

    if(dist > .5) {
        vec2 dir = normalize(normal.xy);
        vec2 tang = vec2(dir.y, -dir.x);
        float threshold = sampleThreshold(angle);
        float next = sampleThreshold(angle + .01);

        if(dist > threshold) {

            float t = (dist - threshold) / (1 - threshold);
            if(t < .1 || t > .9)
                gl_FragColor = vec4(.5,0,0,1);
            else

            gl_FragColor = vec4(1,0,0,1);
        }
    }
}