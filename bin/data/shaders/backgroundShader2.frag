#version 130

precision mediump float;


// Uniforms
uniform sampler2D uCellTexture;
uniform sampler2D uBubbleTexture;
uniform sampler2D uMembraneTexture;
uniform vec2 uResolution;
uniform float uTime;
uniform sampler2D uSpotTexture;
uniform vec3 uBackgroundColor;
//uniform vec4 uBaseColor;
uniform float uCellHueShift;
uniform vec3 uBorderParticleColor;

uniform float uBorderRepsU;
uniform float uBorderRepsV;

// Varying
varying vec2 vScenePosition;
varying vec2 vTexCoord;

const float uAlpha = 1.0;

//vec3 BACKGROUND_COLOR = vec3(1.0, 0.725, 0.725);

#define BACKGROUND_COLOR uBackgroundColor


const float pi = 3.14159265359;
#define uTimeParameter uTime

/*
vec3 sampleHexColor(vec2 xy) {
    float reps = 4.0;
    bool checkX = mod(xy.x * reps + 5. * uTime, 1.0) < .5;
    bool checkY = mod(xy.y * reps + uTime, 1.0) < .5;
    if(checkX ? checkY : !checkY)
        return vec3(1,0,.5);
    return vec3(0,1,.5);
}
*/

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


vec2 getCurvedCoords(vec2 xy) {
    return xy * (1.0 - .15 * dot(xy, xy));
}

vec2 getWobbleCoords(vec2 uv) {
    if(dot(uv, uv) < -.0001)
        return uv;
    else {
        float len = length(uv);
        return uv + (uv / len) * cos(len * 12.0 - 10. * uTime * 2. * pi ) * 0.005;
    }
}

vec4 getCheckColor(vec2 xy) {
  
  float reps = 7.0;
  bool checkX = mod(xy.x * reps, 1.0) < .5;
  bool checkY = mod(xy.y * reps, 1.0) < .5;
  if(checkX ? checkY : !checkY)
    return vec4(.8,.9,.2,1);
  return vec4(.5,.4,.5,1);
}

vec2 getFlowCoord(vec2 xy) {
  float len = length(xy);
  return xy * (1. + .04 * sin(30. * len - uTime * 2. * pi));
}

vec2 getSphereCoord(vec2 xy) {
  vec2 toVec = vec2(xy.x, 1.2 * xy.y);
  float len = length(toVec);
  return xy * (1. + pow(1.5 * len, 3.));
}

vec2 getRotatedCoord(vec2 xy) {
  float angle = uTime * 2. * pi;
  vec2 cs = vec2(cos(angle), sin(angle));
  return vec2(
    xy.x * cs.x - xy.y * cs.y,
    xy.x * cs.y + xy.y * cs.x
  );
}

vec2 refractThroughBubble(vec2 uv, vec2 pos, float radius, inout vec4 overlayColor) {
    vec2 toPos = uv - pos;
    float sqrdDist = dot(toPos, toPos);
    if(sqrdDist < radius * radius) {

        vec2 texUv = .5 + toPos / radius;

        overlayColor = vec4(texUv, 0, 1);
        //vec4 clr = texture2D(uBubbleTexture, );

        float dist = sqrt(sqrdDist);
        //toPos = toPos / dist;
        return uv + toPos * pow(sqrdDist / (radius * radius), 2.);// * dist / radius;
    }
    return uv;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 sampleHexValue(vec2 xy) {
        
    float numHexHeight = 7.0;// 10.0 * mouse.y / 768.0;
    xy.y *= 1.3;
    
    //vec2 xy = (uv - .5) * vec2(1024, 768); //(0,0) is center screen. Each step corresponds to 1 pixel
    
    float hexHeight = 768.0 / numHexHeight; //i.e: 5 stacked on top of each other -> fills screen
    float hexRad = .5 * hexHeight / cos(pi / 6.0);
    float hexWidth = 2.0 * hexRad;
    
    float fragWidth = 3.0 * hexRad;
    float fragHeight = hexHeight;
    
    vec2 baseHexLoc = vec2(0,0);
    float fragX = xy.x / fragWidth;
    baseHexLoc.x = fragX - fract(fragX);
    fragX = fract(fragX);
    
    float fragY = xy.y / fragHeight;
    baseHexLoc.y = fragY - fract(fragY);
    fragY = fract(fragY);
    
    vec2 hexLoc = vec2(0,0);
    
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

    //return vec4(uv, 0, 1);

    vec4 color = texture2D(uCellTexture, uv);

    vec2 actualHexLoc = baseHexLoc + hexLoc;
    float hexDist = sqrt(actualHexLoc.x * actualHexLoc.x + .4 * actualHexLoc.y * actualHexLoc.y);// dot(actualHexLoc, actualHexLoc);

    //distance alpha:
    float distFactor = min(1.0, abs(hexDist / 5.0));
    color.a *= 1. - pow(distFactor, 3.);
    distFactor = pow(distFactor, .5);
    color.a *= distFactor * (.0 + 1. * rand(actualHexLoc)) + (1. - distFactor);

    vec3 hsv = rgb2hsv(color.rgb);
    hsv.x += uCellHueShift;
    color.rgb = hsv2rgb(hsv);
    

    return color;
}

vec2 hash22(vec2 p) { 
    float n = sin(dot(p,vec2(41, 289))); 
    p = fract(vec2(8.0*n, n)*262144.);
    return sin(p*6.2831853 + uTime*2.);
}

float Voronoi3Tap(vec2 p){
    
    vec2 s = floor(p + (p.x+p.y)*0.3660254);
    p -= s - (s.x+s.y)*0.2113249;

    float i = step(0.0, p.x-p.y); 
    vec2 p1 = p - vec2(i, 1.0-i) + 0.2113249, p2 = p - 0.5773502; 

    p += hash22(s)*0.125;
    p1 += hash22(s +  vec2(i, 1.0-i))*0.125;
    p2 += hash22(s + 1.0)*0.125;
    
    float d = min(min(dot(p, p), dot(p1, p1)), dot(p2, p2))/0.425;
   
    return sqrt(d);

}

vec4 getVoronoiColors( vec2 uv )
{
    // Screen coordinates.
	//vec2 uv = (fragCoord.xy - iResolution.xy*0.5)/ iResolution.y;
    
    // Take two 3-tap Voronoi samples near one another.
    float c = Voronoi3Tap(uv*5.);
    float c2 = Voronoi3Tap(uv*5. - 10./uResolution.y);
    
    // Coloring the cell.
    //
    // Use the Voronoi value, "c," above to produce a couple of different colors.
    // Mix those colors according to some kind of moving geometric patten.
    // Setting "pattern" to zero or one displays just one of the colors.
    float pattern = cos(uv.x*0.75*3.14159-0.9)*cos(uv.y*1.5*3.14159-0.75)*0.5 + 0.5;
    
    // Just to confuse things a little more, two different color schemes are faded in out.
    //
    // Color scheme one - Mixing a firey red with some bio green in a sinusoidal kind of pattern.
    vec3 col = mix(vec3(c*1.3, pow(c, 2.), pow(c, 10.)), vec3(c*c*0.8, c, c*c*0.35), pattern );
    // Color scheme two - Farbrausch fr-025 neon, for that disco feel. :)
    vec3 col2 = mix(vec3(c*1.2, pow(c, 8.), pow(c, 2.)), vec3(c*1.3, pow(c, 2.), pow(c, 10.)), pattern );
    // Alternating between the two color schemes.
    col = mix(col, col2, smoothstep(.4, .6, sin(uTime*.25)*.5 + .5)); // 

    //col = mix(col.zxy, col, cos(uv.x*2.*3.14159)*cos(uv.y*5.*3.141595)*.25 + .75 );
    
    // Hilighting.
    //
    // Use a combination of the sample difference "c2-c" to add some really cheap, blueish highlighting.
    // It's a directional-derviative based lighting trick. Interesting, but no substitute for point-lit
    // bump mapping. Comment the following line out to see the regular, flat pattern.
    col += vec3(0.5, 0.8, 1.)*(c2*c2*c2-c*c*c)*5.;

    if(true) {
        float someVal = 5 * (c2*c2*c2-c*c*c);
        return vec4(someVal, 0, 0, 1);
    }
       
    // Speckles.
    //
    // Adding subtle speckling to break things up and give it a less plastic feel.
    col += (length(hash22(uv + uTime))*0.06 - 0.03)*vec3(1., 0.5, 0.);
    

    // Vignette.
    //
    col *= (1.15 - dot(uv, uv)*0.5);//*vec3(1., 0.97, 0.92); // Roundish.
    vec2 p = uv*vec2(uResolution.y/uResolution.x, 1.)+0.5; // Rectangular.
   // col *= smoothstep(0., 0.5, pow( 16.*p.x*p.y*(1.0-p.x)*(1.0-p.y), 0.25))*vec3(1.1, 1.07, 1.01);
    
    
	return vec4(sqrt(clamp(col, 0., 1.)), 1);
	
}

void main(void) {
    vec2 xy = (vTexCoord - .5) * uResolution.xx / uResolution.yx;

    vec4 overlayColor = vec4(1,1,1,0);

    /*
    xy = refractThroughBubble(xy, vec2(0, 0), .15, overlayColor);
    xy = refractThroughBubble(xy, vec2(0.2 - .1 * sin(5. * uTime * 2.0 * pi), 0.1), .13, overlayColor);
    xy = refractThroughBubble(xy, vec2(0.2 - .1 * sin(5. * uTime * 2.0 * pi), -0.2), .11, overlayColor);
    xy = refractThroughBubble(xy, vec2(0.1 - .15 * sin(5. * uTime * 2.0 * pi), -0.4), .09, overlayColor);
    xy = refractThroughBubble(xy, vec2(0.2 - .14 * sin(5. * uTime * 4.0 * pi), 0.2), .07, overlayColor);
    */
    //xy = getWobbleCoords(xy);
    xy = getCurvedCoords(xy);

    //xy = getSphereCoord(xy);
    xy = getRotatedCoord(xy);

    gl_FragColor = sampleHexValue(xy * uResolution);


    //blend bubble clr:
    gl_FragColor.rgb = overlayColor.a * overlayColor.rgb + (1.0 - overlayColor.a) * gl_FragColor.rgb;

    vec3 TUNNEL_SHADE =  vec3(BACKGROUND_COLOR.r * BACKGROUND_COLOR.r, BACKGROUND_COLOR.g * BACKGROUND_COLOR.g, BACKGROUND_COLOR.b * BACKGROUND_COLOR.b);

    gl_FragColor.rgb = (1. - gl_FragColor.a) * BACKGROUND_COLOR + gl_FragColor.a * gl_FragColor.rgb;
    gl_FragColor.a = 1.0;

    float dist = length(xy);
    if(dist < .5)
        dist = 0.0;
    else 
        dist = (dist - .5) * 3.;
    gl_FragColor.rgb = mix(gl_FragColor.rgb, TUNNEL_SHADE, dist);

    //tunnel:
    // vec2 memUv = .5 + .6 * xy;
    // vec4 memClr = vec4(TUNNEL_SHADE, .35) * texture2D(uMembraneTexture, memUv);

    // gl_FragColor.rgb = (1. - memClr.a) * gl_FragColor.rgb + memClr.a * memClr.rgb;


    

    // vec2 uv = (vTexCoord * 2.0 - .5);
    // if(uv.x > 0.0 && uv.x < 1.0 && uv.y > 0.0 && uv.y < 1.0)
    //     gl_FragColor = texture2D(uCellTexture, uv);
    // else
    //     gl_FragColor = vec4(1,.5,0,1);

    //gl_FragColor = vec4(clamp(xy, 0, 1),0,1);

    //alt: move
    //gl_FragColor = sampleHexValue((xy + uTime * vec2(3,1)) * uResolution);

    /*
    //membrane
    float c = Voronoi3Tap(vTexCoord * 5.);
    if(c > .5) {
        float blend = clamp(5. * (c - .5) / .5, 0, 1);
        gl_FragColor.xyz = blend * vec3(1, .6, .2) + (1. - blend) * gl_FragColor.xyz;
    }

    gl_FragColor = getVoronoiColors(xy);
    */

    //blend in some tex collahz!
    vec2 spots_xy = vTexCoord - .5;
    float spotsLocLength = length(spots_xy);

    //blend 
    if(false)
    {
        float u = fract(3. * atan(spots_xy.y, spots_xy.x) / pi + 2. * uTime);
        float v = fract(5. * pow(spotsLocLength, .2) + 1. * uTime);
        float spotAlpha = .15 * (1. - texture2D(uBubbleTexture, vec2(u,v)).r);
        spotAlpha *= pow(4. * spotsLocLength, 2.);
        spotAlpha = clamp(spotAlpha, 0, 1);
        // vec3 hsv = rgb2hsv(gl_FragColor.rgb);
        // hsv.y *= .2; 
        // hsv.z = 1.; 
        vec3 rgb = gl_FragColor.rgb;
        rgb = pow(rgb, vec3(2.0));
        spotAlpha *= .5;
        //rgb.r *= 1.1;
        //rgb.gb *= .85;
        gl_FragColor.rgb = (1. - spotAlpha) * gl_FragColor.rgb + spotAlpha * rgb;// hsv2rgb(hsv);
    }

    float spotLocStartDistance = 0.4;
    if(spotsLocLength > spotLocStartDistance) {
        float u = fract(uBorderRepsU * atan(spots_xy.y, spots_xy.x) / pi + 2. * uTime);
        float v = fract(uBorderRepsV * pow(spotsLocLength, .2) + 4. * uTime);
        float spotAlpha = .7 * (1. - texture2D(uSpotTexture, vec2(u,v)).r);

        spotAlpha *= pow(3. * (spotsLocLength - spotLocStartDistance), 2.);

        // vec3 hsv = rgb2hsv(gl_FragColor.rgb);

        // // if(hsv.x > .1)
        // //     hsv = rgb2hsv(vec3(.4, .3, .35));

        // hsv.x = hsv.x;//fract(hsv.x + .15 * sin((u + 20. * uTime) * 2 * pi ) + (.65 + .3 * pow(spotsLocLength - spotLocStartDistance, .5)));
        // hsv.y = 1.0;
        // hsv.z = 1.0 - .5 * (spotsLocLength - spotLocStartDistance);//clamp(hsv.y + 1. * spotAlpha, 0., 1.);
        //gl_FragColor.rgb = (1. - spotAlpha) * gl_FragColor.rgb + spotAlpha * hsv2rgb(hsv);
        gl_FragColor.rgb = (1. - spotAlpha) * gl_FragColor.rgb + spotAlpha * uBorderParticleColor;// hsv2rgb(hsv);
        //gl_FragColor.rgb = pow(1. - spotAlpha, 1) * gl_FragColor.rgb + spotAlpha * vec3(1);// (.4 * gl_FragColor.rgb);
    }
}