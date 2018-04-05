#version 130

precision mediump float;

// Uniforms
uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform vec4 uBaseColor;
uniform sampler2D uSpotTexture;
uniform int uMode;
uniform float uZoom;
uniform float uWeight;

uniform vec3 uColor1;
uniform vec3 uColor2;
uniform vec3 uColor3;
uniform vec3 uColor4;

// Varying
varying vec2 vScenePosition;
varying vec2 vTexCoord;

const float uAlpha = 1.0;


const float pi = 3.14159265359;
#define uTimeParameter uTime


#define position vTexCoord
#define time uTime

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float n){
 	return fract(cos(n*89.42)*343.42);
}

#define ZOOM uZoom
//was 2.5

#define WGHT uWeight
//was 0.13

#define MODE uMode

float truchet(vec2 uv, float blur)
{
    uv *= ZOOM * uResolution / uResolution.xx;
    vec2 st = floor(uv), p = fract(uv);
    
    if (rand(st) >.5) 
      p.x = 1. - p.x;

    //let (0,0) be center of the truchet circle:
    if(p.x + p.y > 1.0)
      p = 1 - p;

    float p_dist = length(p);
    float circleDistance = 0;//abs(0.5 - length(p));
    if(MODE == 0) {
      circleDistance = abs(0.5 - p_dist);
    }
    else if(MODE == 1) {
      circleDistance = 2 * min(abs(0.4 - p_dist), abs(0.6 - p_dist));
    }
    else if(MODE == 2) {
      circleDistance = 2. * min(abs(0.3 - p_dist), min(abs(0.5 - p_dist), abs(0.7 - p_dist)));
    }
    else if(MODE == 3) {
      circleDistance =  .5 * abs(.5 - (p.x + p.y));
    }
    else if(MODE == 4) {
      circleDistance =  p_dist * .2;
    }
    

    float e = blur / min(uResolution.y , uResolution.x);
    return 1. - smoothstep(-e, e, circleDistance - WGHT);
}

vec3 sqrd_mix(vec3 v1, vec3 v2, float mixval) {

  return sqrt(mix(v1 * v1, v2 * v2, mixval));

}

void main() {
  
  vec2 xy = position - .5;
  float distFromCenter = length(xy);
  xy *= pow(2 * distFromCenter + 1, 0.5);
  vec2 uv = xy + .5 - .3 * vec2(2., 1.4) * time;

  vec3 c1 = mix( uColor1, uColor2, position.y);
  vec3 c2 = mix( uColor3, uColor4, position.y);

  vec4 col = vec4(1,0,0,1);
  float tr = truchet(uv, 1 + 30 * distFromCenter);

  // float spot = 1.0 - texture2D(uSpotTexture, fract(0.8 * vTexCoord * uResolution / uResolution.xx + uTime * vec2(2.0, 1.0))).r;
  // spot *= distFromCenter;
  // c1 *= (1 + .2 * spot);
  // c2 *= (1 - .2 * spot);

  if(tr > .999) {
    col.rgb = c1;
  }
  else {
    float effect = tr / .999;
    tr = truchet(uv + .1 * xy, 10 + 80 * distFromCenter);
    vec3 c = (1 - .2 * tr) * c2;
    col.rgb = sqrd_mix(c, c1, effect);
  }

  gl_FragColor = col;
}