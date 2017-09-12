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

void main() {
  
  vec2 uv = position;

  uv.y = fract(uv.y -   time);
  uv.x = fract(uv.x -   time);
  uv.y = 1 - uv.y;

  //morph to lens coords:
  
  // background
  vec3 c1 = mix( vec3( 0.3, 0.1, 0.3 ), vec3( 0.1, 0.4, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );
  vec3 c2 = mix( vec3( 0.1, 0.3, 0.3 ), vec3( 0.5, 0.3, 0.5 ), dot( position, vec2( 0.2, 0.7 ) ) );

//   vec3 c1 = mix( vec3( 0.5, 0.3, 0.5 ), vec3( 0.9, 0.5, 0.6 ), dot( position, vec2( 0.2, 0.7 ) ) );
//   vec3 c2 = mix( vec3( 0.9, 0.8, 0.3 ), vec3( 0.5, 0.7, 0.8 ), dot( position, vec2( 0.2, 0.7 ) ) );
  
  vec4 col = vec4(1,0,0,1);
  if(onArrow(uv)) {
    col.rgb = c1;
  }
  else {
    if(onArrow(uv - vec2(.000, -.01)))
      col.rgb = .95 * c2;
   	else
      col.rgb = c2;
  }
  
  col.rgb += (rand(position)-.5) * .05;
  
  
  gl_FragColor = col;
  
}