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
varying vec2 vModelPosition;
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

vec3 sqrd_mix(vec3 v1, vec3 v2, float mixval) {

  return sqrt(mix(v1 * v1, v2 * v2, mixval));

}

void main(void) {
    gl_FragColor = vec4(.5 + .5 * vNormal,1);

    vec3 dir = vec3(0,0,1);

    vec3 normal = vNormal;
    normal.z = pow(normal.z, 0.5);
    normal = normalize(normal);
    float offset = .5 + .5 * dot(dir, normal);
    vec3 blobNormal = normal;
    float tiltFactor = .5 + .5 * cos(pow(fract(15 * (offset + uTime)), .5) * 2 * pi);
    blobNormal.z += 0.4 * tiltFactor;
    blobNormal = normalize(blobNormal);


    vec3 lightdir = normalize(vec3(1,-1,4));
    lightdir = normalize(vec3(500,-500,1500) - vec3(vModelPosition, 10 * normal.z + 10. * (1 - length(vModelPosition))));
    lightdir = normalize(vec3(300,-300,1500) - vec3(vScenePosition, 20 * normal.z +  length(vModelPosition) / 10.0));
    float b = dot(lightdir, blobNormal);
    bool flipColor = b < 0.;
    b = pow(b, 2.0);
    ///b = abs(b);
    //bool isBackShade = b < 0;
    //b = pow(b, .9);

    //float hue = rgb2hsv(uLightColor).x;
    vec3 lightColor = uLightColor;//flipColor ? uHighlightColor : uLightColor;
    vec3 darkColor = uDarkColor;//hsv2rgb(vec3(hue, 1, 0.27));
    vec3 highlightColor = vec3(1);//uHighlightColor;//hsv2rgb(vec3(hue, 0.63, 1));


    //gl_FragColor = vec4(b, clamp(b, 0, 1), 0., 1.);


    float highlightThreshold = .95;
    if(b < highlightThreshold) {
        gl_FragColor.rgb = mix(darkColor, lightColor, pow(b / highlightThreshold, 1));
    }
    else {
        gl_FragColor.rgb = mix(lightColor, highlightColor, .5 * pow((b - highlightThreshold) / (1 - highlightThreshold), 3.0));
    }

    if(vOffsetFactor < 0)
        gl_FragColor.rgb = .7 * darkColor;//mix(gl_FragColor.rgb, darkColor, .8);
    else {
        float normLength = length(normal.xy);
        if(normLength > .45) {
            float u = fract(1.4 * atan(normal.y, normal.x) / (2. * pi) + uTime);
            float v = fract(.3 * pow(length(normal.xy), 5.0) - 2. * uTime + .007 * tiltFactor);


            vec4 texClr = texture2D(uSpotTexture,vec2(u,v));
            float effect = 1. - texClr.r;
            effect *= (normLength - .45) / .55;
            effect *= pow(1.3 - b, 1.0);
            gl_FragColor.rgb = mix(gl_FragColor.rgb, darkColor, min(2. * pow(effect, 2.0), .4));
        }

        if(normLength > .8) {
            float effect = (normLength - .8) / .2;
            gl_FragColor.rgb = mix(gl_FragColor.rgb, .8 * darkColor, pow(effect, 4.0));
        }
    }
}