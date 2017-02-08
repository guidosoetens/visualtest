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

void main(void) {

    //vec2 texPosition = vTexCoord;
    vec2 texPosition = .5 + (vTexCoord - .5) * (uResolution / uResolution.yy);
    
    vec2 p = -1.0 + 2.0 * texPosition;
    float len = length(p);
    /*
    if(len > 1.)
        discard;
    */
    
    vec2 uv;
    if(len < .02)
        uv = texPosition;
    else
        uv = texPosition + (p/len) * cos(len * 12.0 - uTimeParameter * pi * 2.0 ) * 0.0075;
    
    vec4 texColor = texture2D(uTexture,uv);
    gl_FragColor = (1.0 - texColor.a) * uBaseColor + texColor.a * texColor;
    gl_FragColor.a = 1.0;

    //blend in some tex collahz!
    vec2 xy = vTexCoord - .5;
    float len = length(xy);
    if(len > 0.) {
        float u = fract(5. * atan2(xy.y, xy.x) / pi);
        float v = fract(3. * len);
        vec4 spotColor = vec4(vec3(1), texture2D(uSpotTexture, vec2(u,v)).r);
        gl_FragColor.rgb = gl_FragColor.rgb - spotColor.a * (1.4 * gl_FragColor.rgb + .3);

        gl_FragColor.rgb = vec3(0.);
    }


/*
        uBaseColor


    gl_FragColor = texture2D(uTexture,uv) * uAlpha;
    */

}