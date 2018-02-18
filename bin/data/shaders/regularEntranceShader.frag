#version 130

precision mediump float;

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform vec2 uResolution;
uniform float uDarken;
uniform int uDarkenBrim;

varying vec2 vScenePosition;
varying vec2 vTexCoord;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void) {
    vec4 tex = texture2D(uTexture, vTexCoord);
    vec3 normal = 2 * (tex.rgb - .5);
    normal.yz = normal.zy;
    normal.y *= -1;
    normal = gl_NormalMatrix * normal;
    normal = normalize(normal);

    vec3 lightdir = normalize(vec3(300,-300,1500) - vec3(vScenePosition, 20 * normal.z));
    lightdir = normalize(vec3(500,-500,-1000) - vec3(vScenePosition, 0 * normal.z));
    //lightdir = vec3(1,1,0);
    float b = .5 + .5 * dot(lightdir, normal);
    if(vTexCoord.y > .5 || b > 0.0)
        b = pow(b, 2.0);

    vec3 darkColor = .3 * uColor;//vec3(.2, .0, .3);
    vec3 midColor = .66 * uColor;//vec3(.5, .05, .3);
    vec3 lightColor = 1.0 * uColor;//vec3(1.0, .1, .4);
    if(b < .5)
        gl_FragColor.rgb = mix(darkColor, midColor, b / .5);
    else
        gl_FragColor.rgb = mix(midColor, lightColor, (b - .5) / .5);

    if(b > .95) {
        float highlight = pow((b - .95) / .05, 4.0);
        gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(1), highlight);
    }

    if(tex.b < .01)
        gl_FragColor.rgb = darkColor;

    gl_FragColor.a = tex.a;

    /*
    vec4 clr = texture2D(uTexture, vTexCoord);
    gl_FragColor = clr;
    //clr.rgb -= uDarkenBrim == 1 ? .5 : 0;

    vec3 hsv = rgb2hsv(clr.rgb);
    //if gray:
    if(hsv.y < .1) {
        float effect = 1.0 - hsv.y / 0.1;
        float gray = clr.r;
        gray = pow(gray, 1.5 + .0 * uDarken);
        gray *= (1 - .6 * uDarken);
        vec3 blendColor;
        if(gray < .5)
            blendColor = mix(vec3(0), uColor, gray / .5);
        else
            blendColor = mix(uColor, vec3(1), (gray - .5) / .5);
        gl_FragColor.rgb = mix(clr.rgb, blendColor, effect);
    }
    else {
        gl_FragColor.rgb -= uDarkenBrim == 1 ? .1 : 0;
    }
    */
}