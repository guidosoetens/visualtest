#version 130

precision mediump float;

varying vec2 vPosition;
varying vec2 vTexCoord;

uniform vec2 uCenter;
uniform vec2 uFocusPoint;

bool isNan(float val)
{
  return (val <= 0.0 || 0.0 <= val) ? false : true;
}

void main() {

    vec2 toPt = 2. * (vTexCoord - .5);
    float d = length(toPt);

    if(d > 1.0)
        discard;


    gl_FragColor = vec4(vTexCoord, 0.5, 1.0);
}
