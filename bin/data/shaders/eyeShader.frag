#version 130

precision mediump float;

varying vec2 vPosition;
varying vec2 vTexCoord;

uniform vec2 uCenter;
uniform vec2 uFocusPoint;
uniform vec2 uPupilLoc;
uniform int uFlipHorizontally;

const float pi = 3.1415926535;
const vec3 lightNormal =  normalize(vec3(4,-4,-1));

bool isNan(float val)
{
  return (val <= 0.0 || 0.0 <= val) ? false : true;
}

vec4 getPupilColor(vec2 xy) {

    float b = .7 * max(0.0, 1. - 2.0 * length(xy - vec2(-.5)));

    if(length(xy) < .65)
        return vec4(b,b,b,1);
    return vec4(b + .3, b + .3, b + .3, 1);
}

void main() {

    vec2 uv = vTexCoord;
    if(uFlipHorizontally == 0)
        uv.x *= 1. + .2 * uv.y;
    else {
        uv.x -= 1.0;
        uv.x *= 1. + .2 * uv.y;
        uv.x += 1.0;
    }

    vec2 xy = 2. * (uv - .5);
    float d = length(xy);

    if(d > 1.0)
        discard;
    if(d > .9) {
        gl_FragColor = vec4(0,0,0,.5);
        return;
    }

    float z = sqrt(1 - d * d);

    gl_FragColor = vec4(.4 + .5 * z, .4 + .5 * z, 1.0, 1.0);
    //gl_FragColor = vec4(1);

    float pupilRadius = 15.0;
    if(length(vPosition - uPupilLoc) < pupilRadius) {
        gl_FragColor = getPupilColor((vPosition - uPupilLoc) / pupilRadius);
    }

    float diffuse = dot(lightNormal, vec3(xy, z));
    gl_FragColor.xyz += .2 * vec3(diffuse);
}
