#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uTimeParameter;
uniform int uRenderFlow;

uniform vec2 uFlowSource;
uniform vec2 uFlowSink1;
uniform vec2 uFlowSink2;

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;

#define time uTimeParameter

float random(float p) {
  return fract(sin(p)*1000.);
}

float noise(vec2 p) {
  return random(p.x + p.y*1000.);
}

vec2 sw(vec2 p) {return vec2( floor(p.x) , floor(p.y) );}
vec2 se(vec2 p) {return vec2( ceil(p.x)  , floor(p.y) );}
vec2 nw(vec2 p) {return vec2( floor(p.x) , ceil(p.y)  );}
vec2 ne(vec2 p) {return vec2( ceil(p.x)  , ceil(p.y)  );}

float smoothNoise(vec2 p) {
  vec2 inter = smoothstep(0., 1., fract(p));
  float s = mix(noise(sw(p)), noise(se(p)), inter.x);
  float n = mix(noise(nw(p)), noise(ne(p)), inter.x);
  return mix(s, n, inter.y);
  return noise(nw(p));
}

float movingNoise(vec2 p) {
  float total = 0.0;
  total += smoothNoise(p     - time);
  total += smoothNoise(p*2.  + time) / 2.;
  total += smoothNoise(p*4.  - time) / 4.;
  total += smoothNoise(p*8.  + time) / 8.;
  total += smoothNoise(p*16. - time) / 16.;
  total /= 1. + 1./2. + 1./4. + 1./8. + 1./16.;
  return total;
}

float nestedNoise(vec2 p) {
  float x = movingNoise(p);
  float y = movingNoise(p + 100.);
  return movingNoise(p + vec2(x, y));
}

void main() {

    //vec2 scale = vec2(1.0, uResolution.y / uResolution.x);
    //vec2 uv = (scale * vPosition / uResolution + 1.0) / 2.0;

    vec3 normal = normalize(vNormal);
    //normal.z = pow(normal.z, 0.1);
    //normal = normalize(normal);
    
    gl_FragColor = vec4(.5 + .5 * normal.xy, .5 + .5 * normal.z, 1);
    
    //float b = .5 + .5 * dot(normal, lightNormal);
    //gl_FragColor = vec4(b, .5 * b, .5, 1);
    //gl_FragColor = vec4(vNormal.z, .3, 0, 1);
    
    if(uRenderFlow == 1) {

        vec2 flowCoord = vFlowCoord;//* vec2(5, 1.5);// + 10 * (1 - vNormal.z));


        float valX = flowCoord.x;//length(flowCoord);// vFlowCoord.x + vFlowCoord.y;
        
        float valY = 0.0;

        valX = flowCoord.x + pow(flowCoord.y, 1.4);
        valX = flowCoord.x;
        valX = length(flowCoord);
        valX = flowCoord.x + pow(flowCoord.y, 2.0);
        valX = flowCoord.x;
        
        vec2 uv = vec2(
            fract(valX - uTimeParameter),
            fract(valY - .25) 
        );
        
        bool checkY = uv.y < .5;
        float r = 1.0;
        if(( uv.x < .5) ? checkY : !checkY)
           gl_FragColor = vec4(1,0,0,1);
        else
           gl_FragColor = vec4(.5,0,0,1);

        //r = .8 + .2 * sin(uv.x * 2 * pi);
        //gl_FragColor = vec4(r, 0, 0, 1);
           


       // r = .5 + .5 * sin(uv.x * 2. * pi);
       // gl_FragColor = vec4(r, .5, 0, 1);
        

          //r = 0;
/*
        vec2 p = (vPosition / uResolution) * 10.0 + .1 * normal.xy;
        float brightness = nestedNoise(p);
        gl_FragColor.rgb = mix(vec3(0,0.2,0), vec3(r,1.0,0), brightness);//c vec3(.5, .4 + .5 * brightness, .5);
        gl_FragColor.a = 1.;
        */
    }
}
