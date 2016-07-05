#version 130

precision mediump float;

uniform vec2 uResolution;
varying vec2 vPosition;
varying vec3 vNormal;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);

void main() {

    vec2 scale = vec2(1.0, uResolution.y / uResolution.x);
    vec2 uv = (scale * vPosition / uResolution + 1.0) / 2.0;

    vec3 normal = vNormal;
    normal.z = pow(normal.z, 0.5);
    normal = normalize(normal);

    float off = .1 * pow(1.0 - normal.z, 3.0);
    vec2 checkerUv = uv + off * normalize(normal.xy);

    vec3 color = vec3(.5, .8, .7);
    bool checkX = fract(checkerUv.x * 30.0) < .5;
    bool checkY = fract(checkerUv.y * 30.0) < .5;
    if(checkX ? checkY : !checkY)
        color *= .75;


    float diffuseIntensity = dot(normal, lightNormal);
    color += diffuseIntensity * vec3(.4);

    if(normal.z < .5)
        color.xyz *= .5 + .5 * normal.z / .5;

    gl_FragColor = vec4(color, 1.0);
}
