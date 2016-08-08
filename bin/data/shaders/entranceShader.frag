#version 130

precision mediump float;

varying vec2 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

const float pi = 3.1415926535;
const vec3 lightNormal =  normalize(vec3(4,-4,-1));

bool isNan(float val)
{
    return (val <= 0.0 || 0.0 <= val) ? false : true;
}

void main() {

    float length = length(vNormal.xy);
    float threshold = .2 * pow(abs(vTexCoord.x), 3.0);

    if(vNormal.z < threshold)
        discard;
    else if(vNormal.z < threshold + .05) 
        gl_FragColor = vec4(0,0,0,1);
    else {
        float blue = vNormal.z < threshold + .3 ? vNormal.z : 1;
         gl_FragColor = vec4(.5 + .5 * vNormal.xy, blue, 1);
    }
}
