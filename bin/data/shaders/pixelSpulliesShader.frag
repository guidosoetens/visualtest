#version 130

precision mediump float;

uniform sampler2D uTex;
uniform vec2 uTexSize;
uniform vec2 uTexStep;

// Uniforms
varying vec2 vTexCoord;

#define BORDER_OFFSET 0.7
#define GREEN_STYLE true

bool differ(vec3 c1, vec3 c2) {
    return length(c1 - c2) > .001;
}

void main(void) {

    //find coord within pixel:
    vec2 innerCoord = 2.0 * (fract(vTexCoord * uTexSize) - .5);
    vec3 c = texture2D(uTex, vTexCoord).rgb;
    gl_FragColor = vec4(c, 1);


    vec2 steps[8];
    steps[0] = vec2(1, 0);
    steps[1] = vec2(1, 1);
    steps[2] = vec2(0, 1);
    steps[3] = vec2(-1, 1);
    steps[4] = vec2(-1, 0);
    steps[5] = vec2(-1, -1);
    steps[6] = vec2(0, -1);
    steps[7] = vec2(1, -1);

    bool isBorder = false;
    float minimumDistance = 1.0;

    for(int i=0; i<8; ++i) {
        vec2 s = steps[i];
        vec3 sample = texture2D(uTex, vTexCoord + s * uTexStep).rgb;
        if(differ(sample, c)) {

            //found differing color in steps[i]-direction
            if(i % 2 == 0) {
                //X:
                if(abs(s.x) > 0) {
                    float dist_x = BORDER_OFFSET * s.x - innerCoord.x;
                    if(s.x < 0.0)
                        dist_x *= -1.0;
                    if(dist_x < 0.0)
                        minimumDistance = min(minimumDistance, dist_x);
                }

                //Y:
                if(abs(s.y) > 0) {
                    float dist_y = BORDER_OFFSET * s.y - innerCoord.y;
                    if(s.y < 0.0)
                        dist_y *= -1.0;
                    if(dist_y < 0.0)
                        minimumDistance = min(minimumDistance, dist_y);
                }
            }
            else {
                //CORNER:

                bool xIsBorder = false;
                float xBorderDist = 1.0;

                //X:
                if(abs(s.x) > 0) {
                    float dist_x = BORDER_OFFSET * s.x - innerCoord.x;
                    if(s.x < 0.0)
                        dist_x *= -1.0;
                    if(dist_x < 0.0) {
                        xIsBorder = true;
                        xBorderDist = dist_x;
                    }
                }

                //Y:
                if(abs(s.y) > 0 && xIsBorder) {
                    float dist_y = BORDER_OFFSET * s.y - innerCoord.y;
                    if(s.y < 0.0)
                        dist_y *= -1.0;
                    if(dist_y < 0.0) {
                        //YES!
                        float minXyDist = min(xBorderDist, dist_y);
                        minimumDistance = min(minimumDistance, minXyDist);
                    }
                }
            }
        }
    }

    //GREEN STYLE
    if(GREEN_STYLE) {
        if(length(c) < .1)
            gl_FragColor = vec4(0,.2,0,1);
        else
            gl_FragColor = vec4(0,.4,0,1);
        if(minimumDistance < 0.0) {
            gl_FragColor = vec4(.5,.9,.2,1);
        }
    }
    else {
        gl_FragColor = vec4(1);
        
        if(minimumDistance < 0.0) {
            gl_FragColor = vec4(.8,.8,.8,1);
            if(minimumDistance > -.1)
                gl_FragColor = vec4(0,0,0,1);
        }

        if(vTexCoord.x < .01 || vTexCoord.y < .01 || vTexCoord.x > .99 || vTexCoord.y > .99)
            gl_FragColor = vec4(0,0,0,1);
    }
}