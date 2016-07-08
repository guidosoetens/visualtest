#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uTimeParameter;
uniform int uRenderFlow;

varying vec2 vPosition;
varying vec3 vNormal;
varying float vDistance;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;

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
        float t = vDistance / 20.0 /*+ .5 * pow(normal.z, 2.0) + 1.08*/ - uTimeParameter + 2 * (1 - normal.z);
        float r = .5 + .5 * sin(t * 2.0 * pi);
        r = fract(t) < .5 ? 1.0 : 0.5;
        gl_FragColor = vec4(r, 1, .5, .5);   
        
    }
}
