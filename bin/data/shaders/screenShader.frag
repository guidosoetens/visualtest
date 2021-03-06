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
    
    vec3 cResult = clr.rgb;
    
    //add scanlines:
    vec2 sc = vec2( sin( vUv.y * sCount ), cos( vUv.y * sCount ) );
    cResult += clr.rgb * vec3( sc.x, sc.y, sc.x ) * sIntensity;
    
    // interpolate between source and result by intensity
    cResult = clr.rgb + clamp( nIntensity, 0.0,1.0 ) * ( cResult - clr.rgb );
    
    float gray = cResult.r * 0.3 + cResult.g * 0.59 + cResult.b * 0.11;
    gray = .3 + .6 * gray;
    cResult = .3 * gray * TERMINAL_COLOR + .15 * cResult;// vec3( cResult.r * 0.3 + cResult.g * 0.6 + cResult.b * 0.3 );
    
    //sampleSmoothGuiBrightness
    float backB = sampleGuiBrightness(vec2(.5) + (1.0 + 0.075 * uZoomStrength) * (vUv - vec2(.5)));
    float b = .1 * backB + sampleSmoothGuiBrightness(vUv);
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
        
        vec2 jUv = uZoomStrength * uv + (1.0 - uZoomStrength) * uv;
        
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
            if(uZoomStrength > .001 && (jUv.x < .1 ||  jUv.y < .1 || jUv.x > .9 || jUv.y > .9)) {

                float v1 = clamp(1. - (abs(.5 - jUv.x) - .4) / .1, 0., 1.);
                float v2 = clamp(1. - (abs(.5 - jUv.y) - .4) / .1, 0., 1.);

                float t = pow(v1 * v2, 1.5);
                vec4 offClr = (1.0 - t) * black + t * bgClr;

                bgClr = (1.0 - uZoomStrength) * bgClr + uZoomStrength * offClr;

                //bgClr = (1.0 - t) * black + t * bgClr;
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