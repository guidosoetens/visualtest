#version 130

precision mediump float;

// Uniforms
uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform vec4 uBaseColor;
uniform sampler2D uSpotTexture;

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


bool onArrow(vec2 uv) {

  vec2 gridCoord = vec2(fract(uv.x * 5.0), fract(uv.y * 5.0));
  if(gridCoord.x < .5) {
    float x = gridCoord.x / .5;
    if(gridCoord.y < .5) {
      float y = gridCoord.y / .5;
      return x / y < 1.0;
    }
    else {
      float y = (gridCoord.y - .5) / .5;
      return x / y > 1.0;
    }
  }
  else {
    float x = 1.0 - (gridCoord.x - .5) / .5;
    if(gridCoord.y < .5) {
      float y = gridCoord.y / .5;
      return x / y < 1.0;
    }
    else {
      float y = (gridCoord.y - .5) / .5;
      return x / y > 1.0;
    }
  }
  
  
  return false;
}

#define ZOOM 6.
#define WGHT .12

// float hash(vec2 p)
// {
//     return fract(sin(dot(p,vec2(127.1,311.7))) * 43758.5453123);
// }

float df_circ(in vec2 p, in vec2 c, in float r)
{
    return abs(r - length(p - c));
}

float sharpen(in float d, in float w)
{
    float e = 1. / min(uResolution.y , uResolution.x);
    return 1. - smoothstep(-e, e, d - w);
}

float df_pattern(vec2 uv)
{
    float l1 = sharpen(df_circ(uv, vec2(0), .5), WGHT);
    float l2 = sharpen(df_circ(uv, vec2(1), .5), WGHT);
    return max(l1,l2);
}

float truchet(vec2 uv)
{
    uv *= ZOOM * uResolution / uResolution.xx;
    //uv += vec2(5 * uTime);

    vec2 st = floor(uv), p = fract(uv);
    if (rand(st) >.5) 
      p.x = 1. - p.x;
    return df_pattern(p);
}

void main() {
  
  vec2 uv = position - time;

  // uv.y = uv.y - time;
  // uv.x = uv.x - time;

  //morph to lens coords:
  
  // background
  vec3 c1 = mix( vec3( 0.3, 0.1, 0.3 ), vec3( 0.1, 0.4, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );
  vec3 c2 = mix( vec3( 0.1, 0.3, 0.3 ), vec3( 0.5, 0.3, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );

  // vec3 c1 = mix( vec3( 0.5, 0.3, 0.5 ), vec3( 0.9, 0.5, 0.6 ), dot( position, vec2( 0.2, 0.7 ) ) );
  // vec3 c2 = mix( vec3( 0.9, 0.8, 0.3 ), vec3( 0.5, 0.7, 0.8 ), dot( position, vec2( 0.2, 0.7 ) ) );
  
  vec4 col = vec4(1,0,0,1);
  if(truchet(uv) > 0) { //onArrow(uv)) {
    col.rgb = c1;
  }
  else {
    if(truchet(uv - vec2(.000, .01)) > 0)
      col.rgb = .8 * c2;
   	else
      col.rgb = c2;
  }
  
  //col.rgb += rand(position - .5) * .05;// (rand(floor(fract(uv) * uResolution / uResolution.xx * uResolution.x))-.5) * .05;
  col.rgb += (rand(floor(uv * uResolution) / uResolution) - .5) * .05;
  gl_FragColor = col;

  // float tru = truchet(position);
  
  // gl_FragColor = col + .03 * tru;
}