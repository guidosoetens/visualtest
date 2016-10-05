#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uMaxDepth;
uniform float uDepthOffset;
uniform float uTime;
uniform float uRevealParameter;
uniform float uBaseHue;
uniform int uDrawMode;
uniform float uWinAnimParameter;

uniform vec2 uSheenFrom;
uniform vec2 uSheenUnitVector;
uniform float uSheenLength;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

//const float sqrt_third = 0.57735026919;
const vec3 lightNormal =  normalize(vec3(4,4,-1));// vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;

/*
const float BASE_HUE_GREEN = .3;
const float BASE_HUE_ORANGE = .1;
const float BASE_HUE = BASE_HUE_ORANGE;
const vec3 BASE_HSV = vec3(BASE_HUE, 1.0, .7);
*/

//vec3 BASE_HSV = vec3(uBaseHue, 1.0, .7);

/*
 const vec4 BASE_COLOR_GREEN = vec4(.1, .65, .15, 1);
 const vec4 BASE_COLOR_ORANGE = vec4(.9, .65, .2, 1);
 const vec4 BASE_COLOR = BASE_COLOR_GREEN;
 */

bool isNan(float val)
{
    return (val <= 0.0 || 0.0 <= val) ? false : true;
}

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

float calcSheenBrightness() {
    vec2 to = (vPosition.xy / uResolution) - uSheenFrom;
    float val = dot(to, uSheenUnitVector) / uSheenLength;
    float sheenFactor = 0.;
    if(val < .52)
        sheenFactor = val / .52;
    else
        sheenFactor = 1. - (val - .52) / .48;
    return clamp(sheenFactor, 0.0, 1.0);
}

void main() {
   
    gl_FragColor = vec4(.5 + .5 * vNormal.x, .5 + .5 * vNormal.y, 0, 1);
   
    float absBlendOffset = .3;
    float additionalOffset = 1.5 + absBlendOffset;
   
    float absDepth = (vFlowCoord.x + uDepthOffset + .5 * pow(max(0.0,vFlowCoord.y), 1.5));
    float relDepth = absDepth / (uMaxDepth + additionalOffset);
   
    float absRevealThreshold = uRevealParameter * (uMaxDepth + additionalOffset);
   
    float postWinFadeFactor = max(0.0, 1.0 - 4.0 * clamp(uWinAnimParameter, 0., 1.));
   
   
   
    float shineFactor = 0.0;
    if(relDepth < uRevealParameter)
        shineFactor = min(1.0, absRevealThreshold - absDepth);
    shineFactor *= postWinFadeFactor;
   
   
    vec3 hsv = vec3(uBaseHue, 1.0, .8);
   
    float alpha = 1.0;
   
    if(uDrawMode == 0) {
       
        //GLOW:
       
        hsv.z += .5;
        vec3 rgb = hsv2rgb(hsv);
       
        float whiteFrac = .5 + .5 * pow(vNormal.z, 1.0);
        rgb = mix(rgb, vec3(1.5), whiteFrac);
       
        gl_FragColor.rgb = rgb;
        gl_FragColor.a = .5 * min(1.0, 2. * shineFactor * vNormal.z);
       
       
        if(uWinAnimParameter > -1.0 && uWinAnimParameter < 1.0) {
           
            float v = 0.0;
            float radEffect = 1.0;
            float radOffset = 1.0;
           
            float v1 = .5;
            float dv2 = 1.0;
            float dist = 1.0 - vNormal.z;
           
            if(uWinAnimParameter < 0.0) {
                //load:
                float t = uWinAnimParameter + 1.0;
                //v = (1. - dist) * (v1 + dv2 * t);
                radOffset = (-.5 + 1.5 * (1. - fract(3. * t))) - pow(dist, .5 - .4 * t);
                radEffect = .3 + .2 * t;
               
            }
            else {
                float t = pow(uWinAnimParameter, .3);
                //v = (1. - 3. * t) * (1. - dist) * (v1 + dv2) ;
                radOffset = (-.5 + 2. * t) - pow(dist, 4.);
                radEffect = 1.0;
            }
           
            v = clamp(v, 0., 1.);
            v += radEffect * pow(1. - dist, .5) * clamp(1. - 10. * abs(radOffset), 0., 1.);
           
            gl_FragColor.a += v;
           
        }
       
        return;
    }
   
    //INTERNAL:
   
    float diffuse = dot(lightNormal, vNormal);
   
    hsv.z += sign(diffuse) * .2 * diffuse * diffuse;
   
    if(relDepth < uRevealParameter) {
       
        float blendFactor = 1.0;
        if(absDepth > absRevealThreshold - absBlendOffset)
            blendFactor = (absRevealThreshold - absDepth) / absBlendOffset;
       
       
        float t = min(1.0, 2. * absRevealThreshold) * .5 + .4 * (1. - vNormal.z) + .05 * sin(30. * absDepth + 50. * (1. - uRevealParameter) - uTime * 2.0 * pi);
        t *= blendFactor;
        t *= postWinFadeFactor;
       
        hsv.x += t * .05;
        float valFrag = t;// t * .5;
        hsv.z = (1. - valFrag) * hsv.z + valFrag * 1.2;
    }
   
    if(uWinAnimParameter > .0001 && uWinAnimParameter < .9999) {
        float b = calcSheenBrightness();
        hsv.z *= 1.0 + .5 * b;
        hsv.y *= 1.0 - .2 * b;
    }
   
   
    //border:
    float borderAlpha = 1.0;
    if(vPosition.z < .3) {
       
        float darkFactor = 1.0;
        if(vPosition.z > .25)
            darkFactor = 1. - (vPosition.z - .25) / .05;
       
        hsv.z *= 1.0 - .3 * darkFactor;
       
       
        if(vPosition.z < .05) {
            borderAlpha = vPosition.z / .05;
        }
    }
   
   
    float innerAlpha = 1.0;
    if(vNormal.z > .3)
        innerAlpha = 1.0 - .2 * (vNormal.z - .3) / .7;
   
   
    gl_FragColor.rgb = hsv2rgb(hsv);
    gl_FragColor.a = innerAlpha * borderAlpha;
   
}