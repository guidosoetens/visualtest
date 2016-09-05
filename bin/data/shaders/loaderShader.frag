#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uTimeParameter;
uniform float uCornerRadius;

varying vec2 vPosition;
varying vec2 vTexCoord;

const float pi = 3.1415926535;

vec4 sampleBarColor() {

    //manhattan distance?
    float mDist = vPosition.x + vPosition.y;
    if(fract(mDist / 60.0 - uTimeParameter) < .33)
        return vec4(0,0,0,1);
    return vec4(.15, .15, .15, 1);

    /*
    vec4 res = vec4(1,1,1,1);
    bool checkX = fract(vTexCoord.x * 5.0 + uTimeParameter) < .5;
    bool checkY = fract(vTexCoord.y * 2.0) < .5;
    if(checkX ? checkY : !checkY) 
        res = vec4(0,0,0,1);
    //return vec4(.2, .5, .5, 1);
    return res;
    */
}



void main() {

    float subRadius = .8 * uCornerRadius;// (.5 + .5 * sin(uTimeParameter * pi * 2.0)) * uCornerRadius;
    float invSubRadius = uCornerRadius - subRadius;


    
    vec2 xy = vTexCoord * uResolution;

    if(xy.x < uCornerRadius || xy.x > uResolution.x - uCornerRadius) {

        if(xy.y < uCornerRadius || xy.y > uResolution.y - uCornerRadius) {
            //sample from corner:
            xy = xy - .5 * uResolution;
            if(xy.x < 0)
                xy.x = -xy.x;
            if(xy.y < 0)
                xy.y = -xy.y;
            xy -= (.5 * uResolution - uCornerRadius);
            float r = length(xy);
            if(r < subRadius)
                gl_FragColor = sampleBarColor();
            else if(r < uCornerRadius)
                gl_FragColor = vec4(0,0,0,1);
            else 
                discard;
                //gl_FragColor = vec4(1,1,1,1);

            //gl_FragColor = sampleBarColor();
        }
        else if(xy.x < invSubRadius || xy.x > uResolution.x - invSubRadius)
            gl_FragColor = vec4(0,0,0,1);
        else
            gl_FragColor = sampleBarColor();
    }
    else if(xy.y < invSubRadius || xy.y > uResolution.y - invSubRadius)
        gl_FragColor = vec4(0,0,0,1);
    else
        gl_FragColor = sampleBarColor();


/*
    gl_FragColor = vec4(1,1,1,1);

    bool checkX = fract(vTexCoord.x * 5.0 + uTimeParameter) < .5;
    bool checkY = fract(vTexCoord.y * 2.0) < .5;
    if(checkX ? checkY : !checkY) 
        gl_FragColor = vec4(0,0,0,1);
        */
}
