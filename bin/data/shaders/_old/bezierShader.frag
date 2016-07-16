#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uTimeParameter;

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;

vec4 sampleColor(vec2 p)
{  
      // Gradients  
      vec2 px = dFdx(p);  
      vec2 py = dFdx(p);  
      
      // Chain rule  
      float fx = (2.0 * p.x) * px.x - px.y;  
      float fy = (2.0 * p.x) * py.x - py.y;  
      
      // Signed distance  
      float sd = (p.x * p.x - p.y) / sqrt(fx * fx + fy * fy);  
      
      if(2.0 - abs(sd) > 0.0)
        return vec4(1,1,1,1);
      else if(20.0 - abs(sd) > 0.0)
        return vec4(1,0,0,1);
      return vec4(0,0,0,0);
}  


void main() {
    
    gl_FragColor = sampleColor(vTexCoord);// vTexCoord.x * vTexCoord.x - vTexCoord.y;

    /*
    float red = 
    
    float red = value;
    float blue = 1 - red;
    
    gl_FragColor = vec4(red,0,blue,1);
    */
}