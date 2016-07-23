#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uMaxDepth;
uniform float uDepthOffset;
uniform float uTime;
uniform float uRevealParameter;

uniform int uDrawMode;

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;

const vec4 BASE_COLOR_GREEN = vec4(.1, .65, .15, 1);
const vec4 BASE_COLOR_ORANGE = vec4(.9, .65, .2, 1);
const vec4 BASE_COLOR = BASE_COLOR_GREEN;

bool isNan(float val)
{
  return (val <= 0.0 || 0.0 <= val) ? false : true;
}

/*
vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;  

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
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

void main() {
    
    //NOTE: additional .5 offset from the 'y'-offset
    float absDepth = (vFlowCoord.x + uDepthOffset + .5 * pow(vFlowCoord.y, 1.5));
    float relDepth = absDepth / (uMaxDepth + 0.5);

    gl_FragColor = BASE_COLOR;

    vec3 light = normalize(vec3(4,-4,-1));

    float diffuse = dot(light, vNormal);
 
    gl_FragColor.xyz += vec3(sign(diffuse) * .4 * diffuse * diffuse);

    if(uDrawMode == 0) {

        vec3 hsv = rgb2hsv(BASE_COLOR.xyz);
        hsv.z = uRevealParameter * 2.5 * pow(vNormal.z, 2.0);

        //float a = uRevealParameter * 1. * (vNormal.z);
        gl_FragColor.rgb = hsv2rgb(hsv);
        return;
    }

    else if(uDrawMode == 1) {
        vec3 hsv = rgb2hsv(BASE_COLOR.xyz);
        hsv.z *= .5;
        gl_FragColor.rgb = hsv2rgb(hsv);
        return;
    }

    if(relDepth < uRevealParameter) {

        float absRevealThreshold = uRevealParameter * (uMaxDepth + 0.5); 

        float t = min(1.0, 2. * absRevealThreshold) * .5 + .3 * (1. - vNormal.z) + .2 * sin(20. * absDepth + 50. * pow(1. - uRevealParameter, 1.0) - 10.0 * uTime);
        vec3 hsv = rgb2hsv(BASE_COLOR.xyz);

        hsv.x += t * .05;
        hsv.y = (1. - t) * hsv.y + t;
        float valFrag = t;// t * .5;
        hsv.z = (1. - valFrag) * hsv.z + valFrag * 1.4;

        gl_FragColor.rgb = hsv2rgb(hsv);

        /*
        //gl_FragColor.rgb = t + (1 - t) * BASE_COLOR.xyz;

        float glitterLength = 2. - 2. * uRevealParameter;
        //float absRevealThreshold = uRevealParameter * (uMaxDepth + 0.5); 

        if(absDepth > absRevealThreshold - glitterLength) {

            float strength = 1. - (absRevealThreshold - absDepth) / glitterLength;

            vec2 uv = vPosition / uResolution.yy;
            vec3 pos = vec3(150. * uv, 3.5 * uTime);
            float b = (1.5 + 5.0 * pow(strength, 3.0) ) * snoise( pos );

            float n = .5 * smoothstep(.3, 1., b * b);
            
            //gl_FragColor.xyz += strength * n;
        }
        */
    }

}
