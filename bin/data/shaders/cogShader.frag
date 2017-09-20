#version 130

precision mediump float;

uniform float uRotation;
uniform sampler2D uBumpMap;
uniform float uRadius;

// Varying
varying vec2 vModelPosition;
varying vec2 vScenePosition;
varying vec3 vNormal;

const float pi = 3.14159265359;
const float sqrtHalf = 0.70710678118;
const float sqrtThird = 0.57735026919;

vec2 rotate2D(vec2 vec, float angle) {
    float cs = cos(angle);
    float sn = sin(angle);
    return vec2(cs * vec.x - sn * vec.y, sn * vec.x + cs * vec.y);
}

void main(void) {

    vec3 normal = vNormal;
    float darken = 0;
    if(normal.z < 1.0) {
        //edge:
        normal.z  = .8 + normal.z;
        normal = normalize(normal);
    }
    else {
        vec2 uv = fract(vModelPosition / 400.0);
        vec4 bumpColor = texture2D(uBumpMap, uv);
        normal = 2 * bumpColor.rgb - 1;
        normal.xy = rotate2D(normal.xy, uRotation);
        darken = 1 - pow(normal.z, 5.);
        normal.z = 2 + normal.z * 3.0;
        normal = normalize(normal);

        float dist = length(vModelPosition);
        if(dist > 0) {
            vec2 dir = vModelPosition / dist;
            float threshold = uRadius / 2.0;
            if(dist > threshold && dist < threshold + 4) {
                float t = (dist - threshold) / 4.0;
                float tiltAngle = -.2 * (.5 - .5 * cos(t * 2 * pi));

                float angle = atan(dir.y, dir.x) + uRotation;
                normal.xy = rotate2D(normal.xy, angle);
                normal.yz = rotate2D(normal.yz, tiltAngle);
                normal.xy = rotate2D(normal.xy, -angle);
            }

            threshold = uRadius / 3.0;
            if(dist > threshold && dist < threshold + 4) {
                float t = (dist - threshold) / 4.0;
                float tiltAngle = .2 * (.5 - .5 * cos(t * 2 * pi));

                float angle = atan(dir.y, dir.x) + uRotation;
                normal.xy = rotate2D(normal.xy, angle);
                normal.yz = rotate2D(normal.yz, tiltAngle);
                normal.xy = rotate2D(normal.xy, -angle);
            }
        }

    }

    gl_FragColor = vec4(.5 + .5 * normal, 1);

    vec3 lightDir = normalize(vec3(1,-1,2));
    lightDir = vec3(uRadius, -uRadius, 100) - vec3(rotate2D(vModelPosition, uRotation), 0);
    lightDir = normalize(lightDir);
    float b = .5 + .5 * dot(lightDir, normal);
    b = pow(b, 2.);

    vec3 darkColor = vec3(0.1,.1,.2);
    vec3 midColor = vec3(.5, .3, .3);
    vec3 lightColor = vec3(1, 1, .8);

    b = .2 + .8 * b;

    if(b < .5)
        gl_FragColor.rgb = mix(darkColor, midColor, b / .5);
    else
        gl_FragColor.rgb = mix(midColor, lightColor, (b - .5) / .5);

    //gl_FragColor *= (1 - .2 * darken);

    gl_FragColor.a = 1;// = vec4(b,b,b,1);
}