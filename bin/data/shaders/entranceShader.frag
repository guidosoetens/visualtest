#version 130

precision mediump float;

varying vec2 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec4 vColor;

uniform vec3 uFaceNormal;

const float pi = 3.1415926535;
const vec3 lightNormal =  normalize(vec3(4,-4,-1));

bool isNan(float val)
{
    return (val <= 0.0 || 0.0 <= val) ? false : true;
}

void main() {

    vec3 normal = normalize(vNormal);

    vec3 up = vec3(0,1,0);
    vec3 u = normalize(cross(up, uFaceNormal));
    vec3 v = cross(uFaceNormal, u);

    float offset = dot(uFaceNormal, normal);

    //float length = length(vNormal.xy);
    float threshold = .1 * pow(abs(vTexCoord.x), 2.0);


    if(true) {
        float red = .5 + .5 * dot(vNormal, u);
        float green = .5 + .5 * dot(vNormal, v);
        gl_FragColor = vec4(red, green, offset, 1);
        return;
    }
    
    if(offset < threshold)
        discard;
    else if(offset < threshold + .2) 
        gl_FragColor = vec4(0,0,0,1);
    else {

        float red = .5 + .5 * dot(vNormal, u);
        float green = .5 + .5 * dot(vNormal, v);

        float blue = offset < threshold + .3 ? offset : 1;
        gl_FragColor = vec4(red, green, blue, 1);
    }
    

    //gl_FragColor = vColor;
}
