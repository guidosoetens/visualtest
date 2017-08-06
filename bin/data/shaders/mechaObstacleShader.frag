#version 130

precision mediump float;

// Uniforms
uniform sampler2D uTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform vec4 uBaseColor;
uniform sampler2D uSpotTexture;

uniform vec3 uDarkColor;
uniform vec3 uLightColor;
uniform vec3 uHighlightColor;

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

vec2 rotate2D(vec2 xy, float angle) {
  vec2 cs = vec2(cos(angle), sin(angle));
  return vec2(
    xy.x * cs.x - xy.y * cs.y,
    xy.x * cs.y + xy.y * cs.x
  );
}

vec4 getSpotColor() {

    float effect = 1.0 - normal.z;
    if(effect < .7)
        return vec4(0);

    float stretchFactor = pow(1. - pow(1. - effect * effect, 1.), 1.5);

    float scale = 1.0;
    float x = fract(scale * atan(normal.y, normal.x) / pi - 2. * uTime);
    float y = fract(scale * 0.2 * stretchFactor - 5. * uTime);
    if(y < 0.)
        return vec4(1);
    float b = 1. - texture2D(uSpotTexture, vec2(x, y)).r;
    b *= .25 * pow((effect - .7) / .3, 1.5);// * pow(effect, 8.0);
    return vec4(1, 1, 1, b);
}

vec4 sampleHexValue(vec2 xy) {

    //  /  \
    // |    |
    //  \  /
        
    //float numHexHeight = 7.0;
    
    //vec2 xy = (uv - .5) * vec2(1024, 768); //(0,0) is center screen. Each step corresponds to 1 pixel

    // xy.x *= 0.8;
    // xy.y *= 2.0;
    // xy *= 0.8;

    xy.x *= 10.0;
    xy.y = (1.0 - xy.y) * 1.0;



    
    float hexHeight = 1.0;//768.0 / numHexHeight; //i.e: 5 stacked on top of each other -> fills screen
    float hexRad = .5 * hexHeight;// .5 * hexHeight / cos(pi / 6.0);
    float hexWidth = 1.0;//2.0 * hexRad * cos(pi / 6.0);// 2.0 * hexRad;
    
    float fragWidth = hexWidth;//  3.0 * hexRad;
    float fragHeight = hexWidth;//hexRad;//3.0 * hexRad; //hexHeight;
    
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
    
    vec2 uv = .5 + .5 * (hexLoc - vec2(fragX, fragY)) * vec2(1.5, 2.5) * 0.8; // * vec2(2.4);// * vec2(3.0, 2.0);
    uv.x = 1.0 - uv.x;
    uv.y = 1.0 - uv.y;
    

    //uv.x = 1. - uv.x;

    //return vec4(uv, 0, 1);

    if(hexLoc.y < 0.5)
        return vec4(0,0,0,0);
    return texture2D(uCellTexture, uv);


    // vec3 normalizedNormal = normalize(normal);
    // vec3 u = cross(vec3(0., 1., 0.), normalizedNormal);
    // u = normalize(u);
    // vec3 v = cross(normalizedNormal, u);
    // vec3 calcNormal = hexVal.r * u + hexVal.g * v + hexVal.b * normalizedNormal;

    // float effect = dot(normalizedNormal, calcNormal);

    // gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(1,1,1), effect);
}

float getCrystalSample(float x, float y) {
    const float speed = 100.0;
	const float scale = 40.0;
	const int N_MAX = 20;
	const float n = 7.0;
	
	// accumulate n waves
	float S = 0.0;
	for( int i=0; i < N_MAX; ++i){
	
		// allow for n+1 waves
		if( i >= (int(n)+1) )
			break;
		
		float theta = pi * float(i)/float(n);
		
		float wp = x*cos(theta) + y*sin(theta);
		float w = sin(wp*scale + uTime * speed);
		
		// fade out the final wave in proportion to n-floor(n)
		if( i == int(n) )
			w *= n-floor(n);		
		
		// accumulate 
		S += 3.0 * w / n;
	}
	
	return S;
}

vec3 getCrystalNormal(vec2 loc, vec3 normal) {

    loc *= 3.0;
	
    float c = getCrystalSample(loc.x, loc.y);
    float dx = dFdx(c);
    float dy = dFdy(c);

    float strength = 2.0;//.5 + .25 * sin(uTime * 50);
    normal.xz = rotate2D(normal.xz, strength * pow(-dy, 1.0));
    normal.yz = rotate2D(normal.yz, strength * pow(dx, 1.0));
    return normal;
}


void main(void) {

	vec3 normal = normalize(vNormal);


	gl_FragColor = vec4(.3,0,0,1);
    vec2 position = scenePosition / uResolution;
    float effect = 1.0 - vNormal.z;
    vec2 to = normalize(vNormal.xy);
    float stretchFactor = pow(1. - pow(1. - effect * effect, .3), .5);
    vec2 sampleXy = position + .2 * to * stretchFactor;// + vec2(-uTime, uTime);

    vec3 crystalNormal = getCrystalNormal(sampleXy, normal);
    //gl_FragColor.rgb = c;
    //gl_FragColor = vec4(c,0,0,1);

    vec3 lightdir = normalize(vec3(1,-1,0.5));
    float b = .5 + .5 * dot(lightdir, crystalNormal);

    gl_FragColor = vec4(b, .5 * b, 0., 1.);


    float highlightThreshold = 0.95;
    if(b < highlightThreshold) {
        gl_FragColor.rgb = mix(uDarkColor, uLightColor, pow(b / highlightThreshold, 1));
    }
    else {
        gl_FragColor.rgb = mix(uLightColor, uHighlightColor, pow((b - highlightThreshold) / (1 - highlightThreshold), 3.0));
    }



	vec2 sampleCoord = vec2(fract(atan(normal.y, normal.x) / pi) - 1.0 * uTime, vOffsetFactor / .3);
	if(sampleCoord.y > 0.0 && sampleCoord.y < 1.0) {

        sampleCoord.y = pow(sampleCoord.y, .8);
        vec4 texColor = sampleHexValue(sampleCoord);
        texColor.xyz = 2 * (texColor.xyz - .5);


        float normalAngle = atan(normal.y, normal.x);
        //note that the bump maps are rotated, so the rotation parameters must be rotated similarly:
        texColor.xy = rotate2D(texColor.xy, normalAngle + .5 * pi);

        //calculate u,v frame:
        vec3 u = cross(vec3(0., 1., 0.), vNormal);
        u = normalize(u);
        vec3 v = cross(vNormal, u);

        vec3 calcNormal = texColor.r * u + texColor.g * v + texColor.b * vNormal;
        calcNormal = normalize(calcNormal);

        
        vec3 resultColor = vec3(0);

        float d = dot(calcNormal, lightdir);
        d = .5 + .5 * d;
        d = pow(d, 1.0);

        vec3 plateDark = .6 * uDarkColor + vec3(.1, 0,0);
        vec3 plateLight = .8 * uLightColor;

        if(d < highlightThreshold) {
            resultColor = mix(plateDark, plateLight, pow(d / highlightThreshold, 1));
        }
        else {
            resultColor = mix(plateLight, uHighlightColor, pow((d - highlightThreshold) / (1 - highlightThreshold), 3.0));
        }

        resultColor = mix(resultColor, vec3(0), .25);

        gl_FragColor.xyz = mix(gl_FragColor.rgb, resultColor, texColor.a);
	}


	if(vOffsetFactor < 0) {
		gl_FragColor.xyz *= 0.2;
	}
	else if(vOffsetFactor < .05) {
		gl_FragColor.xyz *= .9;
	}
	else if(vOffsetFactor < .15) {
		gl_FragColor.xyz *= .9 + .1 * (vOffsetFactor - .05) / .1;
	}

}