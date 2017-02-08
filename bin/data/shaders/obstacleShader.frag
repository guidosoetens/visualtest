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
varying vec3 vNormal;
varying float vOffsetFactor;

const float HIGHLIGHT_HUE = 0.77;
const float BASE_HUE = 0.8;
const float SHADOW_HUE = 0.83;

#define uCellTexture uTexture

const vec3 uLightPosition = vec3(.5, -.5, 1.0);
const vec3 uLightVector = normalize(vec3(1,-1,1));

// Varying
#define scenePosition vScenePosition
#define normal vNormal
//varying float effect;

const float pi = 3.14159265359;
const float cellSamples = 20.;
const float sqrtHalf = 0.70710678118;
const float sqrtThird = 0.57735026919;

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

vec4 sampleHexValue(vec2 xy) {

    //  /  \
    // |    |
    //  \  /
        
    float numHexHeight = 7.0;
    
    //vec2 xy = (uv - .5) * vec2(1024, 768); //(0,0) is center screen. Each step corresponds to 1 pixel

    xy.x *= 0.8;
    xy.y *= 2.0;
    xy *= 0.8;


    
    float hexHeight = 768.0 / numHexHeight; //i.e: 5 stacked on top of each other -> fills screen
    float hexRad = .5 * hexHeight;// .5 * hexHeight / cos(pi / 6.0);
    float hexWidth = 2.0 * hexRad * cos(pi / 6.0);// 2.0 * hexRad;
    
    float fragWidth = hexWidth;//  3.0 * hexRad;
    float fragHeight = 3.0 * hexRad; //hexHeight;
    
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
    if(fragY < div6) {
        if(fragX > .5)
            hexLoc.x += 1.0;
    }
    else if(fragY < 2.0 * div6) {
        if(fragX < .5) {
            if(fragY > div6 * (2.0 - 2.0 * fragX)) {
                hexLoc += 0.5;
            }
        }
        else {
            if(fragY > div6 * (2.0 * fragX)) {
                hexLoc += 0.5;
            }
            else {
                hexLoc.x += 1.0;
            }
        }
    }
    else if(fragY < 4.0 * div6) {
        hexLoc += 0.5;
    }
    else if(fragY < 5.0 * div6) {
        if(fragX < .5) {
            if(fragY > div6 * (4.0 + 2.0 * fragX)) {
                hexLoc.y += 1.0;
            }
            else {
                hexLoc += 0.5;
            }
        }
        else {
            if(fragY < div6 * (6.0 - 2.0 * fragX)) {
                hexLoc += 0.5;
            }
            else {
                hexLoc += 1.0;
            }
        }
    }
    else {
        hexLoc.y += 1.0;
        if(fragX > .5)
            hexLoc.x += 1.0;
    }
    
    vec2 uv = .5 + .5 * (hexLoc - vec2(fragX, fragY)) * vec2(2.4);// * vec2(3.0, 2.0);
    uv.x = 1.0 - uv.x;

    //uv.x = 1. - uv.x;

    //return vec4(uv, 0, 1);

    return texture2D(uCellTexture, uv);
}

vec4 sampleHexValue2(vec2 xy) {

    //   ___
    // /     \
    // \ ___ /
        
    float numHexHeight = 7.0;// 10.0 * mouse.y / 768.0;
     xy.y *= 1.2;
    // xy = (xy - .5);
    // xy = vec2(-xy.y, xy.x);
    // xy = .5 + xy;
    
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

    uv.x = 1. - uv.x;

    //return vec4(uv, 0, 1);

    return texture2D(uCellTexture, uv);
}

vec3 sampleSurfaceBumpNormal()
{
    float effect = 1.0 - vNormal.z;
    
    //get position from { [0,1], [0,1] }
    vec2 position = scenePosition / uResolution;
    
    //calculate sample location:
    vec2 to = normalize(normal.xy);
    float stretchFactor = pow(1. - pow(1. - effect * effect, .3), .5);
    vec2 sampleLoc = position + .2 * to * stretchFactor + vec2(-uTime, uTime);
    
    /*
    //convert sample pos to texture coordinate:
    vec2 texCoords = vec2(
                          mod(sampleLoc.x * cellSamples, 1.),
                          mod(sampleLoc.y * cellSamples * uResolution.y / uResolution.x, 1.)
                          );
    
    //sample and set color:
    texCoords.y = 1.0 - texCoords.y;
    */
    vec4 texColor = sampleHexValue(1500 * sampleLoc * vec2(1, .5));// texture2D(uCellTexture, texCoords);
    
    //calculate u,v frame:
    vec3 normalizedNormal = normalize(normal);
    vec3 u = cross(vec3(0., 1., 0.), normalizedNormal);
    u = normalize(u);
    vec3 v = cross(normalizedNormal, u);
    vec3 calcNormal = texColor.r * u + texColor.g * v + texColor.b * normalizedNormal;
    
    float calcEffect = 1.0 - effect * effect;
    return normalize(calcEffect * calcNormal + (1.0 - calcEffect) * normal);
}

#define INTENSITY 6.5
#define GLOW 2.0

float blob(vec2 uv, vec2 speed, vec2 size, float time) {
	vec2 point = vec2(
		sin(speed.x * time) * size.x,
		cos(speed.y * time) * size.y
	);

	float d = 1.0 / distance(uv, point);
	d = pow(d / INTENSITY, GLOW);
	
	return d;
}

float metaball(vec2 uv) {

	float time = 200.0 * uTime;// iGlobalTime * 0.75;

	float blobsValue = 0.0;// vec4(0.0, 0.0, 0.0, 1.0);
	blobsValue += blob(uv, vec2(1.7, 2.2), vec2(0.4, 0.1), time);
	blobsValue += blob(uv, vec2(1.2, 2.3), vec2(0.3, 0.2), time);
	blobsValue += blob(uv, vec2(2.3, 2.1), vec2(0.2, 0.3), time);
	blobsValue += blob(uv, vec2(2.1, 1.0), vec2(0.1, 0.4), time);

	return blobsValue;
}

vec4 getSpotColor() {

    float effect = 1.0 - normal.z;
    float stretchFactor = pow(1. - pow(1. - effect * effect, .3), .5);

    float scale = 2.0;
    float x = fract(scale * atan(normal.y, normal.x) / pi - 2. * uTime);
    float y = fract(scale * 0.5 * stretchFactor - 10. * uTime);
    if(y < 0.)
        return vec4(1);
    float b = 1. - texture2D(uSpotTexture, vec2(x, y)).r;
    b *= .4 * pow(length(normal.xy), 8.0);
    return vec4(1, 1, 1, b);
}

void main(void) {

    vec3 bumpNormal = sampleSurfaceBumpNormal();
    
    /*
    vec3 hsvColor = rgb2hsv(uBaseColor.xyz);
    vec3 lightColor = hsv2rgb(hsvColor * vec3(1, 1, 1.3));
    vec3 darkColor = hsv2rgb(vec3(.5 * hsvColor.x + .5 * .6, 1, 0.5));


    /*
    lightColor = vec3(1., .05, .3);
    darkColor = vec3(.2, .0, .5);
    vec3 highlightColor = vec3(1., .5, .3);
    */

/*
    vec2 uv = normalize(normal.xy) * (1. - vOffsetFactor);//  vScenePosition / 200.0;
    float ballEffect = .5 * clamp(metaball(uv), 0., 1.);
    */

    vec3 darkColor = vec3(.3, .15, .8);
    vec3 lightColor = vec3(1., .1, .3);
    vec3 highlightColor = vec3(1., .5, .4);

    float calcEffect = max(0., .5 * (1. + dot(bumpNormal, uLightVector)));
    calcEffect = clamp(calcEffect, 0., 1.);
    //vec4 backColor = vec4((1.0 - calcEffect) * darkColor + calcEffect * lightColor, 1);
    vec4 backColor = vec4(mix(darkColor, lightColor, calcEffect), 1);
    
    if(vOffsetFactor < 0) {
        backColor.xyz *= 0.5;
    }
    else if(vOffsetFactor < .05) {
        backColor.xyz *= .8;
    }
    else if(vOffsetFactor < .15) {
        backColor.xyz *= .8 + .2 * (vOffsetFactor - .05) / .1;
    }

    backColor.rgb = mix(backColor.rgb, highlightColor,  pow(calcEffect, 20.0)); 

    /*
    //slight glow effect:
    for(int i=0;i<2;++i) {
        float threshold = i == 0 ? .01 : .06;
        float brightness = i == 0 ? .07 : .03;
        if(vOffsetFactor > threshold) {
            float t = 1. - (vOffsetFactor - threshold) / .1;

            float angle = atan(normal.y, normal.x);
            t += .9 + .1 * sin(6. * angle + 50 * uTime * pi) * sin(2. * angle + 80 * uTime * pi);
            if(t < 1.) {
                t = clamp(t, 0., 1.);
                backColor.rgb += brightness * t;
            }
        } 
    }*/
    
    
    //backColor += 0.4 * pow(calcEffect, 10.0);

    gl_FragColor = backColor;

    //uSpotTexture
    vec4 spotColor = getSpotColor();
    if(vOffsetFactor < .05)
        spotColor *= vOffsetFactor / .05;

    //gl_FragColor.rgb = (1. - spotColor.a) * gl_FragColor.rgb + spotColor.a * spotColor.rgb;
    gl_FragColor.rgb = gl_FragColor.rgb + spotColor.a * (.8 * gl_FragColor.rgb + .7);
}