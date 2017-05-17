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

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

const float F3 =  0.3333333;
const float G3 =  0.1666667;

float simplex3d(vec3 p) {
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 
	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 
	 /* 2. find four surflets and store them in d */
	 vec4 w, d;
	 
	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;
	 
	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

float noise(vec3 m) {
    return   0.5333333*simplex3d(m)
			+0.2666667*simplex3d(2.0*m)
			+0.1333333*simplex3d(4.0*m)
			+0.0666667*simplex3d(8.0*m);
}

float electro()
{
    vec2 xy = .5 + .5 * vNormal.xy;
    //xy.y *= 1 + 1 * pow(vNormal.x, .5);
    //xy += .5;


  //vec2 uv = fragCoord.xy / iResolution.xy;    
  vec2 uv = xy * 2. -1.;  
 
  //vec2 p = fragCoord.xy/iResolution.x;
  vec2 p = xy / 1.0;
  vec3 p3 = vec3(p, uTime*10.0);    
    
  float intensity = noise(vec3(p3*12.0+12.0));
                          
  float t = clamp((uv.x * -uv.x * 0.16) + 0.15, 0., 1.);                         
  float y = abs(intensity * -t + uv.y);
    
  float g = pow(y, 0.2);
                          
//   vec3 col =  vec3(1.48, 1.70, 1.78);// vec3(1.70, 1.48, 1.78);
//   col = col * -g + col;                    
//   col = col * col;
//   col = col * col;

  return max(0, 1 - 1.2 * abs(vNormal.x)) * (1 - g);//vec4(col, 1 - g);
}


float curveSample(float x, float minY, float maxY) {
	float y = sqrt(1 - pow(2 * x - 1, 2));
	return minY + y * (maxY - minY);
}


void main(void) {

	vec3 normal = normalize(vNormal);

	float dist = length(normal.xy);

	gl_FragColor = vec4(0, .5, 1, pow(dist, 10.0));

    //create base color:
    float elect = electro();
    gl_FragColor = mix(gl_FragColor, .8 * vec4(1.3,1.2,2.5,1), 1.3 * pow(1.5 * elect, 1.5));

	if(dist > .93 || true) {

		//gl_FragColor = vec4(1,0,0,1);
		
		//draw blobs:
		float angFactor = fract(10 * atan(normal.y, normal.x) / pi);
		float threshold = 1 - .02 * (1 - pow(2 * angFactor - 1, 2));// 1 - .05 * sin(angFactor * pi);
		dist = (dist - .95) / .05;

		bool onUnit = false;

		if(dist > .96) {
			//sample end curve:
			float edge = curveSample(angFactor, .96, 1);
			gl_FragColor = vec4(1,1,1,0);
			if(dist < edge) {
				onUnit = true;
			}
		}
		else {
			//sample end curve:
			float edge = curveSample(angFactor, .98, .95);
			if(dist > edge) {
				onUnit = true;
			}
		}

		if(onUnit)
			gl_FragColor = vec4(0,.3,.14,1);

	}

}