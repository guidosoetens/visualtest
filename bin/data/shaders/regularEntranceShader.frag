#version 130

precision mediump float;

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform vec2 uResolution;

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

    vec4 clr = texture2D(uTexture, vTexCoord);
    gl_FragColor = clr;

    vec3 hsv = rgb2hsv(clr.rgb);
    //if gray:
    if(hsv.y < .1) {
        float effect = 1.0 - hsv.y / 0.1;
        float gray = clr.r;
        gray = pow(gray, 2.0);
        vec3 blendColor;
        if(gray < .5)
            blendColor = mix(vec3(0), uColor, gray / .5);
        else
            blendColor = mix(uColor, vec3(1), (gray - .5) / .5);
        gl_FragColor.rgb = mix(clr.rgb, blendColor, effect);
    }
}