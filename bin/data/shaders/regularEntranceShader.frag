#version 130

precision mediump float;

uniform sampler2D uTexture;
uniform float uHueShift;

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
    vec3 hsv = rgb2hsv(clr.rgb);
    hsv.x += uHueShift;
    //hsv.z = 2.0 * hsv.z - .5;
    //hsv.z *= 0.9;
    //hsv.z *= 1.4;// += uHueShift;
    //hsv.z *= .8;
    gl_FragColor.rgb = hsv2rgb(hsv);
    //gl_FragColor.rgb = gl_FragColor.rbg;
    gl_FragColor.a = clr.a;
}