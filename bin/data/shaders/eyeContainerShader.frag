#version 130

precision mediump float;

uniform int uShadowMode;
uniform float uAnimParam;
uniform sampler2D uTexture;

varying vec2 vScenePosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

vec2 rotate2D(vec2 vec, float angle) {
    float cs = cos(angle);
    float sn = sin(angle);
    return vec2(cs * vec.x - sn * vec.y, sn * vec.x + cs * vec.y);
}

void main(void) {

    if(uShadowMode == 1) {
        gl_FragColor = vec4(.15,.1,.2,1);
        return;
    }
    else if(uShadowMode == 2) {
        gl_FragColor = vec4(0,0,0,.3);
        return;
    }   

    vec3 normal = normalize(vNormal);

    vec3 lightdir = normalize(vec3(300,-300,1500) - vec3(vScenePosition, 20 * normal.z));
    lightdir = normalize(vec3(500,-500,500) - vec3(vScenePosition, 0 * normal.z));
    float b = dot(lightdir, normal);
    if(vTexCoord.y > .5 || b > 0.0)
        b = pow(b, 2.0);


    vec3 darkColor = vec3(.2, .0, .3);
    vec3 midColor = vec3(.5, .05, .3);
    vec3 lightColor = vec3(1.0, .1, .4);
    if(b < .5)
        gl_FragColor.rgb = mix(darkColor, midColor, b / .5);
    else
        gl_FragColor.rgb = mix(midColor, lightColor, (b - .5) / .5);

    if(b > .9) {
        float highlight = pow((b - .9) / .1, 4.0);
        gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(1), highlight);
    }

    //uTexture
    vec2 uv = vec2(fract(1. * vTexCoord.x + 1. * uAnimParam), fract(.1 * vTexCoord.y + 2. * uAnimParam + .0 * pow(length(normal.xy), 2.)));
    //vec2 uv = vec2(fract(.5 * vTexCoord.x), fract(.05 * vTexCoord.y));
    float d = 1 - texture2D(uTexture, uv).r;
    gl_FragColor.rgb = mix(gl_FragColor.rgb, .5 * darkColor, .3 * d);
    gl_FragColor.a = 1.0;

    if(vTexCoord.y < .3)
        gl_FragColor.rgb *= .2 + .8 * vTexCoord.y / .3;

    
    // bool checkX = fract(uv.x * 14.0) < .5;
    // bool checkY = fract(uv.y * 1.0) < .5;
    // if(checkX ? checkY : !checkY)
    //     gl_FragColor.rgb *= .5;
}