#version 130

precision mediump float;


// Uniforms
//uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
//uniform vec4 uBaseColor;

// Varying
varying vec2 vScenePosition;
varying vec2 vTexCoord;

const float uAlpha = 1.0;


const float pi = 3.14159265359;
#define uTimeParameter uTime


vec3 sampleHexColor(vec2 xy) {
    float reps = 10.0;
    bool checkX = mod(xy.x * reps + 2. * uTime, 1.0) < .5;
    bool checkY = mod(xy.y * reps + uTime, 1.0) < .5;
    if(checkX ? checkY : !checkY)
        return vec3(1,0,.5);
    return vec3(0,1,.5);
}

vec2 getCurvedCoords(vec2 xy) {
    return xy * (1.0 - .3 * dot(xy, xy));
}

vec2 getWobbleCoords(vec2 uv) {
    if(dot(uv, uv) < -.0001)
        return uv;
    else {
        float len = length(uv);
        return uv + (uv / len) * cos(len * 12.0 - 10. * uTime * 2. * pi ) * 0.005;
    }
}

vec2 refractThroughBubble(vec2 uv, vec2 pos, float radius) {
    vec2 toPos = uv - pos;
    float sqrdDist = dot(toPos, toPos);
    if(sqrdDist < radius * radius) {
        float dist = sqrt(sqrdDist);
        //toPos = toPos / dist;
        return uv + toPos * pow(sqrdDist / (radius * radius), 2.);// * dist / radius;
    }
    return uv;
}

vec3 sampleHexValue(vec2 xy) {
        
    float numHexHeight = 8.0;// 10.0 * mouse.y / 768.0;
    xy.y *= 1.3;
    
    //vec2 xy = (uv - .5) * vec2(1024, 768); //(0,0) is center screen. Each step corresponds to 1 pixel
    
    float hexHeight = 768.0 / numHexHeight; //i.e: 5 stacked on top of each other -> fills screen
    float hexRad = .5 * hexHeight / cos(pi / 6.0);
    float hexWidth = 2.0 * hexRad;
    
    float fragWidth = 3.0 * hexRad;
    float fragHeight = hexHeight;
    
    vec2 hexLoc = vec2(0,0);
    float fragX = xy.x / fragWidth;
    hexLoc.x = fragX - fract(fragX);
    fragX = fract(fragX);
    
    float fragY = xy.y / fragHeight;
    hexLoc.y = fragY - fract(fragY);
    fragY = fract(fragY);
    
    hexLoc = vec2(0,0);
    
    //offset hexLoc:
    float div6 = 1.0 / 6.0;
    if(fragX < div6) {
        if(fragY > .5)
            hexLoc.y += 1.0;
    }
    else if(fragX < 2.0 * div6) {
        if(fragY < .5) {
            if(fragX > div6 * (2.0 - 2.0 * fragY)) {
                hexLoc.x += 0.5;
                hexLoc.y += 0.5;
            }
        }
        else {
            if(fragX > div6 * (2.0 * fragY)) {
                hexLoc.x += 0.5;
                hexLoc.y += 0.5;
            }
            else {
                hexLoc.y += 1.0;
            }
        }
    }
    else if(fragX < 4.0 * div6) {
        hexLoc.x += 0.5;
        hexLoc.y += 0.5;
    }
    else if(fragX < 5.0 * div6) {
        if(fragY < .5) {
            if(fragX > div6 * (4.0 + 2.0 * fragY)) {
                hexLoc.x += 1.0;
            }
            else {
                hexLoc.x += 0.5;
                hexLoc.y += 0.5;
            }
        }
        else {
            if(fragX < div6 * (6.0 - 2.0 * fragY)) {
                hexLoc.x += 0.5;
                hexLoc.y += 0.5;
            }
            else {
                hexLoc.x += 1.0;
                hexLoc.y += 1.0;
            }
        }
    }
    else {
        hexLoc.x += 1.0;
        if(fragY > .5)
            hexLoc.y += 1.0;
    }
    
    vec2 uv = .5 + .5 * (hexLoc - vec2(fragX, fragY)) * vec2(3.0, 2.0);
    return vec3(uv, .1);
}

void main(void) {
    vec2 xy = (vTexCoord - .5) * uResolution.xx / uResolution.yx;
    xy = refractThroughBubble(xy, vec2(0, 0), .15);
    xy = refractThroughBubble(xy, vec2(0.2 - .1 * sin(5. * uTime * 2.0 * pi), 0.1), .1);
    xy = refractThroughBubble(xy, vec2(0.2 - .1 * sin(5. * uTime * 2.0 * pi), -0.2), .1);
    //xy = getWobbleCoords(xy);
    xy = getCurvedCoords(xy);
    gl_FragColor = vec4(sampleHexValue((xy + uTime * vec2(3,1)) * uResolution), 1);
}