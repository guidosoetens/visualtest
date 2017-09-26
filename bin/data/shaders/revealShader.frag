#version 130

precision mediump float;

uniform sampler2DRect uTexture;
uniform sampler2DRect uPerlinMask;

uniform float uRevealParam;
uniform vec2 uTextureResolution;
uniform vec2 uPerlinResolution;

varying vec2 vTexCoord;


void main(void) {

    gl_FragColor = texture2DRect(uTexture, vTexCoord);

    if(uRevealParam < 1.0) {

        //sample perlin noise:
        vec2 xy = vTexCoord - .5 * uTextureResolution;
        float centerOffset = length(xy) / (.5 * length(uTextureResolution));
        float coordScale = max(uTextureResolution.x / uPerlinResolution.x, uTextureResolution.y / uPerlinResolution.y);
        xy = (coordScale * xy) + .5 * uPerlinResolution;
        float perlin = texture2DRect(uPerlinMask, xy).r;


        float threshold = (-1 + 3 * uRevealParam) - (centerOffset * (2 - uRevealParam));

        float alpha = 0;
        if(threshold > perlin)
            alpha = 1.0;
        else {
            alpha = 1 - max(0, (perlin - threshold));
        }

        gl_FragColor.a *= alpha;
    }

    if(vTexCoord.y < 5.0) {
        float xThr = uRevealParam * uTextureResolution.x;
        if(vTexCoord.x < xThr) {
            gl_FragColor = vec4(1,0,0,1);
        }
    }
}