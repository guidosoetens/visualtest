#version 130

precision mediump float;

// Uniforms
uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform vec4 uBaseColor;
uniform sampler2D uSpotTexture;

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

#define ZOOM 2.5
#define WGHT .16

// float hash(vec2 p)
// {
//     return fract(sin(dot(p,vec2(127.1,311.7))) * 43758.5453123);
// }

float df_circ(in vec2 p, in vec2 c, in float r)
{
    return abs(r - length(p - c));
}

float sharpen(in float d, in float w, float blur)
{
    float e = blur / min(uResolution.y , uResolution.x);
    return 1. - smoothstep(-e, e, d - w);
}

float df_pattern(vec2 uv, float blur)
{
  float weight = WGHT;// + .03 * sin(50 * uTime);

    float l1 = sharpen(df_circ(uv, vec2(0), .5), weight, blur);
    float l2 = sharpen(df_circ(uv, vec2(1), .5), weight, blur);
    return max(l1,l2);
}

float truchet(vec2 uv, float blur)
{
    uv *= ZOOM * uResolution / uResolution.xx;
    //uv += vec2(5 * uTime);

    vec2 st = floor(uv), p = fract(uv);
    if (rand(st) >.5) 
      p.x = 1. - p.x;
    return df_pattern(p, blur);
}

vec3 sqrd_mix(vec3 v1, vec3 v2, float mixval) {

  return sqrt(mix(v1 * v1, v2 * v2, mixval));

}

void main() {
  
  vec2 xy = position - .5;
  float distFromCenter = length(xy);
  xy *= pow(2 * distFromCenter + 1, 0.5);
  vec2 uv = xy + .5 - .3 * vec2(2., 1.4) * time;

  // uv.y = uv.y - time;
  // uv.x = uv.x - time;

  //morph to lens coords:
  
  // background
  //vec3 c1 = sqrd_mix( vec3( 0.3, 0.1, 0.3 ), vec3( 0.1, 0.4, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );
  //vec3 c2 = sqrd_mix( vec3( 0.1, 0.3, 0.3 ), vec3( 0.6, 0.3, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );

  //vec3 c1 = sqrd_mix( uColor1, uColor2, position.y);//dot( position, vec2( 0.2, 0.7 ) ) );
  //vec3 c2 = sqrd_mix( uColor3, uColor4, position.y);//dot( position, vec2( 0.2, 0.7 ) ) );

    vec3 c1 = mix( uColor1, uColor2, position.y);//dot( position, vec2( 0.2, 0.7 ) ) );
  vec3 c2 = mix( uColor3, uColor4, position.y);//dot( position, vec2( 0.2, 0.7 ) ) );

// c1 = mix( vec3( 0.3, 0.1, 0.3 ), vec3( 0.1, 0.4, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );
// c2 = mix( vec3( 0.1, 0.3, 0.3 ), vec3( 0.6, 0.3, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );

  vec4 col = vec4(1,0,0,1);
  float tr = truchet(uv, 1 + 40 * distFromCenter);
  if(tr > .999) {
    col.rgb = c1;
  }
  else {
    float effect = tr / .999;
    tr = truchet(uv + .1 * xy, 10 + 80 * distFromCenter);
    vec3 c = (1 - .1 * tr) * c2;
   // col.rgb = mix(c, c1, effect);
    col.rgb = sqrd_mix(c, c1, effect);
  }



  // if(truchet(uv) > 0) { //onArrow(uv)) {
  //   col.rgb = c1;
  // }
  // else {
  //   if(truchet(uv + .1 * xy) > 0)
  //     col.rgb = .95 * c2;
  //  	else
  //     col.rgb = c2;
  // }
  
  //col.rgb += rand(position - .5) * .05;// (rand(floor(fract(uv) * uResolution / uResolution.xx * uResolution.x))-.5) * .05;
  //col.rgb += (rand(floor(uv * uResolution) / uResolution) - .5) * (1. * distFromCenter + .25) * .05;
  gl_FragColor = col;

  // float tru = truchet(position);
  
  // gl_FragColor = col + .03 * tru;
}