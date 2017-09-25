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

    vec4 clr = vec4(0);
    for(int i=-2; i<3; ++i) {
        for(int j=-2; j<3; ++j) {
            vec4 c = texture2D(uTexture, vTexCoord + vec2(i, j) / uResolution);
            clr += c;
            // clr.r += pow(c.r, 0.5);
            // clr.g += pow(c.g, 0.5);
            // clr.b += pow(c.b, 0.5);
            // clr.a += pow(c.a, 0.5);
        }
    }

    clr = clr / 25.0;

    // clr.r = pow(clr.r, 2.0) / 25.0;
    // clr.g = pow(clr.g, 2.0) / 25.0;
    // clr.b = pow(clr.b, 2.0) / 25.0;
    // clr.a = pow(clr.a, 2.0) / 25.0;

    vec3 hsv = rgb2hsv(clr.rgb);
    if(hsv.x < .05) {
        //cast color:
        vec3 hsvGoal = rgb2hsv(uColor);
        hsv.x = hsvGoal.x;
    }
    gl_FragColor = vec4(hsv2rgb(hsv), clr.a);
}