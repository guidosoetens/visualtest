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
   
    /*
     gl_FragColor = vec4(1,0,0,1);
     return;
     */
   
    //NOTE: additional .5 offset from the 'y'-offset
   
   
    float absBlendOffset = .3;
    float additionalOffset = 1.5 + absBlendOffset;
   
    float absDepth = (vFlowCoord.x + uDepthOffset + .5 * pow(max(0.0,vFlowCoord.y), 1.5));
    float relDepth = absDepth / (uMaxDepth + additionalOffset);
   
    float absRevealThreshold = uRevealParameter * (uMaxDepth + additionalOffset);
   
    float postWinFadeFactor = max(0.0, 1.0 - 6.0 * uWinAnimParameter);
   
   
   
    float shineFactor = 0.0;
    if(relDepth < uRevealParameter)
        shineFactor = min(1.0, absRevealThreshold - absDepth);
    shineFactor *= postWinFadeFactor;
   
   
   
   
    vec3 hsv = vec3(uBaseHue, 1.0, .8);
   
    float alpha = 1.0;
   
    if(uDrawMode == 0) {
       
        hsv.z += .5;
        vec3 rgb = hsv2rgb(hsv);
       
        float whiteFrac = .5 + .5 * pow(vNormal.z, 1.0);
        rgb = mix(rgb, vec3(2), whiteFrac);
       
        gl_FragColor.rgb = rgb;
        gl_FragColor.a = .5 * min(1.0, 2. * shineFactor * vNormal.z);
       
        //gl_FragColor = vec4(1, 1, 1, .5 * vNormal.z);
        /*
        hsv.z = shineFactor * 1.2 * pow(vNormal.z, 2.0);
        gl_FragColor.rgb = hsv2rgb(hsv);
        gl_FragColor.a = 1.0;
        */
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

// precision mediump float;

// uniform vec2 uResolution;
// uniform float uMaxDepth;
// uniform float uDepthOffset;
// uniform float uTime;
// uniform float uRevealParameter;

// uniform int uDrawMode;

// varying vec3 vPosition;
// varying vec3 vNormal;
// varying vec2 vFlowCoord;

// //const float sqrt_third = 0.57735026919;
// const vec3 lightNormal =  normalize(vec3(4,-4,-1));// vec3(sqrt_third, -sqrt_third, sqrt_third);
// const float pi = 3.1415926535;

// const float BASE_HUE_GREEN = .3;
// const float BASE_HUE_ORANGE = .1;
// const float BASE_HUE = BASE_HUE_ORANGE;
// const vec3 BASE_HSV = vec3(BASE_HUE, 1.0, .7);

// /*
// const vec4 BASE_COLOR_GREEN = vec4(.1, .65, .15, 1);
// const vec4 BASE_COLOR_ORANGE = vec4(.9, .65, .2, 1);
// const vec4 BASE_COLOR = BASE_COLOR_GREEN;
// */

// bool isNan(float val)
// {
//   return (val <= 0.0 || 0.0 <= val) ? false : true;
// }

// vec3 rgb2hsv(vec3 c)
// {
//     vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//     vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//     vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

//     float d = q.x - min(q.w, q.y);
//     float e = 1.0e-10;
//     return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
// }

// vec3 hsv2rgb(vec3 c)
// {
//     vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
//     vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
//     return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
// }

// void main() {
    
//     //NOTE: additional .5 offset from the 'y'-offset
//     float absDepth = (vFlowCoord.x + uDepthOffset + .5 * pow(vFlowCoord.y, 1.5));
//     float relDepth = absDepth / (uMaxDepth + 0.5);


//     float absTransitionDistance = 0.1;

//     float absRevealThreshold = uRevealParameter * (uMaxDepth + 0.5); 

// /*
//     if(absDepth < absRevealThreshold - absTransitionDistance) {

//     }
//     else (absDepth < absTransitionDistance) {

//     }
//     else {
        
//     }*/


//     float shineFactor = 0.0;
//     if(relDepth < uRevealParameter) 
//         shineFactor = min(1.0, absRevealThreshold - absDepth);



//     vec3 hsv = BASE_HSV;

//     float alpha = 1.0;

//     if(uDrawMode == 0) {

//         //GLOW:
//         //hsv.z = vPosition.z * 1.5;// * 2.5 * pow(vNormal.z, 2.0);//   shineFactor * 1.5 * pow(vNormal.z, 2.0);
//         //hsv.z = shineFactor * 1.2 * pow(vNormal.z, 2.0);
//         hsv.z *= 3.0;
//         alpha = shineFactor * 1.2 * pow(vNormal.z, 2.0);
//        // alpha = hsv.z;
//     }
//     else if(uDrawMode == 1) {

//         //BORDER OUTLINE:
//         hsv.z *= .5 + shineFactor * 0.8;

//     }
//     else {
//       //INTERNAL:

//         float diffuse = dot(lightNormal, vNormal);
//         hsv.z += sign(diffuse) * .4 * diffuse * diffuse;

//         if(relDepth < uRevealParameter) {

//             //float absRevealThreshold = uRevealParameter * (uMaxDepth + 0.5); 

//             float t = min(1.0, 2. * absRevealThreshold) * .5 + .4 * (1. - vNormal.z) + .05 * sin(30. * absDepth + 50. * (1. - uRevealParameter));

//             hsv.x += t * .05;
//             //hsv.y = (1. - t) * hsv.y + t;
//             float valFrag = t;// t * .5;
//             hsv.z = (1. - valFrag) * hsv.z + valFrag * 1.3;

//             if(vFlowCoord.y < .5)
//                 alpha = 1.0 - 1.0 * vFlowCoord.y / .5;

//             //gl_FragColor.rgb = hsv2rgb(hsv);
//         }
//     }


//     gl_FragColor.rgb = hsv2rgb(hsv);
//     gl_FragColor.a = alpha;



// /*

//     gl_FragColor = BASE_COLOR;

//     //vec3 light = normalize(vec3(4,-4,-1));

//     float diffuse = dot(lightNormal, vNormal);
 
//     gl_FragColor.xyz += vec3(sign(diffuse) * .4 * diffuse * diffuse);

//     if(uDrawMode == 0) {

//         vec3 hsv = rgb2hsv(BASE_COLOR.xyz);
//         hsv.z = uRevealParameter * 2.5 * pow(vNormal.z, 2.0);

//         //float a = uRevealParameter * 1. * (vNormal.z);
//         gl_FragColor.rgb = hsv2rgb(hsv);
//         return;
//     }

//     else if(uDrawMode == 1) {
//         vec3 hsv = rgb2hsv(BASE_COLOR.xyz);
//         hsv.z *= .5;
//         gl_FragColor.rgb = hsv2rgb(hsv);
//         return;
//     }

//     if(relDepth < uRevealParameter) {

//         float absRevealThreshold = uRevealParameter * (uMaxDepth + 0.5); 

//         float t = min(1.0, 2. * absRevealThreshold) * .5 + .3 * (1. - vNormal.z) + .2 * sin(20. * absDepth + 50. * pow(1. - uRevealParameter, 1.0) - 10.0 * uTime);
//         vec3 hsv = rgb2hsv(BASE_COLOR.xyz);

//         hsv.x += t * .05;
//         hsv.y = (1. - t) * hsv.y + t;
//         float valFrag = t;// t * .5;
//         hsv.z = (1. - valFrag) * hsv.z + valFrag * 1.4;

//         gl_FragColor.rgb = hsv2rgb(hsv);
//     }*/

// }
