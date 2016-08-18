#version 130

precision mediump float;

// Uniforms
uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform vec4 uBaseColor;

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


vec3 sampleSurfaceBumpNormal()
{
    float effect = 1.0 - vNormal.z;
    
    //get position from { [0,1], [0,1] }
    vec2 position = scenePosition / uResolution;
    
    //calculate sample location:
    vec2 to = normalize(normal.xy);
    float stretchFactor = pow(1. - sqrt(1. - effect * effect), 1.5);
    vec2 sampleLoc = position + .2 * to * stretchFactor + vec2(-uTime, uTime);
    
    //convert sample pos to texture coordinate:
    vec2 texCoords = vec2(
                          mod(sampleLoc.x * cellSamples, 1.),
                          mod(sampleLoc.y * cellSamples * uResolution.y / uResolution.x, 1.)
                          );
    
    //sample and set color:
    texCoords.y = 1.0 - texCoords.y;
    vec4 texColor = texture2D(uCellTexture, texCoords);
    
    //calculate u,v frame:
    vec3 normalizedNormal = normalize(normal);
    vec3 u = cross(vec3(0., 1., 0.), normalizedNormal);
    u = normalize(u);
    vec3 v = cross(normalizedNormal, u);
    vec3 calcNormal = texColor.r * u + texColor.g * v + texColor.b * normalizedNormal;
    
    float calcEffect = 1.0 - effect * effect;
    return normalize(calcEffect * calcNormal + (1.0 - calcEffect) * normal);
}

void main(void) {

    vec3 bumpNormal = sampleSurfaceBumpNormal();
    
    
    vec3 hsvColor = rgb2hsv(uBaseColor.xyz);
    vec3 lightColor = hsv2rgb(hsvColor * vec3(1, 1, 1.3));
    vec3 darkColor = hsv2rgb(vec3(.5 * hsvColor.x + .5 * .6, 1, 0.5));
    float calcEffect = max(0., .5 * (1. + dot(bumpNormal, uLightVector)));
    vec4 backColor = vec4((1.0 - calcEffect) * darkColor + calcEffect * lightColor, 1);
    
    if(vOffsetFactor < 0.0) {
        backColor.xyz *= 0.7;
    }
    
    backColor += 0.5 * pow(calcEffect, 10.0);

    
    
    gl_FragColor = backColor;
}