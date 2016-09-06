#version 130

precision mediump float;

// Uniforms
uniform vec2 uResolution;
uniform float uTime;
uniform float uZoomStrength;
uniform vec3 uTerminalColor;

uniform sampler2D uFrontTexture;
uniform sampler2D uBackTexture;

// Varying
varying vec2 texPosition;

const float pi_2 = 6.28318530718;

const float barrelPower = 2.0;

const float distortion = 0.3;
const float distortion2 = 0.6;
const float speed = 0.02;

const float nIntensity = 0.1;
const float sIntensity = 0.8;
const float sCount = 800.0; //4096

const float pi = 3.14159265359;

const int kernelSideWidth = 1;
const int kStart = -kernelSideWidth;
const int kEnd = kernelSideWidth + 1;
const int kMaxWeight = 1 + 2 * kernelSideWidth;

#define TERMINAL_COLOR uTerminalColor

/*
const vec3 TERM_ORANGE = vec3(1.0, 0.7, 0.4);    //ORANGE    (2.5)
const vec3 TERM_BLUE = vec3(0.3, 0.85, 1.0);   //BLUE
const vec3 TERM_GREEN = vec3(0.3, 1.0, 0.5);    //GREEN
const vec3 TERM_PINK = vec3(0.95, 0.8, 1.0);   //LIGHTPINK
const vec3 TERMINAL_COLOR = TERM_ORANGE;
*/

// Start Ashima 2D Simplex Noise

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

bool isCheckered(vec2 pt)
{
    bool testY = fract(pt.y * 10.) < .5;
    if(fract(pt.x * 10.) < .5)
        return testY;
    return !testY;
}

vec2 convertToJitterUv(vec2 uv) {
    
    float ty = uTime*speed;
    float yt = (1.0 - uv.y) - ty;
    float offset = snoise(vec2(yt*3.0,0.0))*0.2;
    offset = offset*distortion * offset*distortion * offset;
    offset += snoise(vec2(yt*50.0,0.0))*distortion2*0.001;
    return vec2(clamp(uv.x + offset, 0.0, 1.0), uv.y);
    
}

float sampleGuiBrightness(vec2 sUv) {
    return texture2D(uFrontTexture, sUv).r;
}

float sampleSmoothGuiBrightness(vec2 uv) {
    
    vec2 kStep = vec2(0.5 / uResolution.x, 0.5 / uResolution.y);
    
    float sumVal = 0.0;
    float sumWeight = 0.0;
    
    for(int i=kStart; i<kEnd; ++i) {
        
        for(int j=kStart; j<kEnd; ++j) {
            
            vec2 sUv = uv + vec2(j, i) * kStep;
            float b = sampleGuiBrightness(sUv);
            int weight = kMaxWeight;
            weight -= (i < 0) ? -i : i;
            weight -= (j < 0) ? -j : j;
            //weight = weight * weight * weight;
            
            if(i == 0 && j == 0)
                weight *= 5;
            
            
            sumVal += b * float(weight);
            sumWeight += float(weight);
        }
    }
    
    return sumVal / sumWeight;
}

vec4 scanlineEffect(vec4 clr, vec2 vUv) {
    
    /*
    vec2 xy = 2.0 * vUv - 1.0;
    vec2 bgUv = (1.0 + 0.2 * uZoomStrength) * (.5 * xy) + .5;
    
    vec4 clr = texture2D(uBackTexture, bgUv);// vec4(1.0, .7, .8, 1.);// texture2D(uBackground, vUv);
    */
    
    /*
    if(clr.a < 1.0)
        discard;*/
    
    //clr.xyz /= clr.a;// * clr.xyz + vec3(1.0 - clr.a);
    
    //C = B + ((D-B)/a)
    
    /*
    // make some noise
    float x = vUv.x * vUv.y * uTime *  1000.0;
    x = mod( x, 13.0 ) * mod( x, 123.0 );
    float dx = mod( x, 0.01 );
    */
    
    // add noise
    vec3 cResult = clr.rgb;// + clr.rgb * clamp( 0.1 + dx * 100.0, 0.0, 1.0 );
    
    // get us a sine and cosine
    vec2 sc = vec2( sin( vUv.y * sCount ), cos( vUv.y * sCount ) );
    
    // add scanlines
    cResult += clr.rgb * vec3( sc.x, sc.y, sc.x ) * sIntensity;
    
    // interpolate between source and result by intensity
    cResult = clr.rgb + clamp( nIntensity, 0.0,1.0 ) * ( cResult - clr.rgb );
    
    
    float gray = cResult.r * 0.3 + cResult.g * 0.59 + cResult.b * 0.11;
    gray = .3 + .6 * gray;
    cResult = .3 * gray * TERMINAL_COLOR + .15 * cResult;// vec3( cResult.r * 0.3 + cResult.g * 0.6 + cResult.b * 0.3 );
    
    
    vec2 hudUv = vUv;
    /*
    AUTO-SCROLL: beetje kut...
     
    float timeFrac = fract(uTime / 10.0);
    if(timeFrac < 0.05)
        hudUv += vec2(.5 - .5 * cos(pi * timeFrac / 0.05), 0.0);
    hudUv.x = fract(hudUv.x);
    */
    
    //cResult = clr.xyz;
    
    //sampleSmoothGuiBrightness
    float backB = sampleGuiBrightness(vec2(.5) + (1.0 + 0.075 * uZoomStrength) * (hudUv - vec2(.5)));
    float b = .1 * backB + sampleSmoothGuiBrightness(hudUv);// max(texture2D(uMenuTexture,  hudUv).x, texture2D(uTextTexture,  hudUv).x);
    b = 2.0 * clamp(b * (.6 + .4 * sc.y * sc.x), 0.0, 1.0);
    cResult = b * TERMINAL_COLOR + (1.0-b) * cResult;
    
    return  vec4( cResult, clr.a );
}

/*
// Given a vec2 in [-1,+1], generate a texture coord in [0,+1]
vec2 Distort(vec2 p)
{
    float theta  = atan(p.y, p.x);
    float radius = length(p);
    radius = pow(radius, barrelPower);
    p.x = radius * cos(theta);
    p.y = radius * sin(theta);
    return p;
}
*/

void main( void )
{
    vec2 uv = texPosition;//vec2(texPosition.x, texPosition.y);
    vec2 xy = (2.0 * uv - 1.0);
    
    float zoom = 0.5 * uZoomStrength;// 1.0 + 1.0 * sin(fract(uTime / 3.0) * 2.0 * pi);// fract(uTime / 3.0);// 1.0;//mouse.y / 200.0;//resolution.x;
    
    float ratio = uResolution.x / uResolution.y;
    vec2 tex = xy;
    float k = zoom * 0.01;//distortion;
    float kcube = zoom * 0.25;//cubicDistortion;
    float r2 = ratio * ratio * (tex.x) * (tex.x) + (tex.y) * (tex.y);
    float f = 1.0 + r2 * (k + kcube * sqrt(r2));
    
    uv = f * (tex.xy * 0.5) + 0.5;
    
    
    xy = (2.0 * uv - 1.0);
    
    /*
    float distance = length(xy);
    float maxDist = distance / max(abs(xy.x), abs(xy.y));
    float offset = abs(maxDist - distance);
    */

    vec4 black = vec4(0, 0, 0, 1);
    
    if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        gl_FragColor = black;
    else {
        
        vec2 jUv = uZoomStrength * /*convertToJitterUv(uv)*/uv + (1.0 - uZoomStrength) * uv;
        
        //vec4 tvClr = getTvColor(uv);
        
        vec2 xy = 2.0 * jUv - 1.0;
        vec2 bgUv = (1.0 + 0.05 * uZoomStrength) * (.5 * xy) + .5;
        
        vec4 bgClr;
        if(bgUv.x < 0.0 || bgUv.x > 1.0 || bgUv.y < 0.0 || bgUv.y > 1.0)
           bgClr = vec4(0,0,0,1);
        else {
            bgClr = texture2D(uBackTexture, bgUv);
            bgClr.a = 1.0;

            //adjust color with black fall-off:
            if(jUv.x < .1 ||  jUv.y < .1 || jUv.x > .9 || jUv.y > .9) {

                float v1 = clamp(1. - (abs(.5 - jUv.x) - .4) / .1, 0., 1.);
                float v2 = clamp(1. - (abs(.5 - jUv.y) - .4) / .1, 0., 1.);

                float t = pow(v1 * v2, 1.5);
                bgClr = (1.0 - t) * black + t * bgClr;
            }
        }
        
        gl_FragColor = scanlineEffect(bgClr, jUv);
        
        if(uZoomStrength < 0.8) {
            
            float f1 = uZoomStrength / .8;
            gl_FragColor = f1 * gl_FragColor + (1.0 - f1) * bgClr;
        }
        
        /*
        if(offset < .02) {
            float b = 1.0 - offset / .02;
            gl_FragColor.xyz = b * vec3(0.0) + (1.0 - b) * gl_FragColor.xyz;
        }
        */
        
        //gl_FragColor = texture2D(uBackTexture, uv);
    }
    
    /*
    if(isCheckered(texPosition))
        gl_FragColor = vec4(1,0,0,1);
    else
        gl_FragColor = texture2D(uFrontTexture, uv);
    */
}