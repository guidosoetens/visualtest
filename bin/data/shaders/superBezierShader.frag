// Created by Per Bloksgaard/2014
// I was inspired by https://www.shadertoy.com/view/XsX3zf but instead of a fast 
// distance approximation, I wanted the exact distance to a quadratic bezier spline.

//Blog article about this shader below. (In danish)
//http://www.hinnerup.net/permanent/2014/01/23/bezier_spline_shader/

//Find roots using Cardano's method. http://en.wikipedia.org/wiki/Cubic_function#Cardano.27s_method
vec3 solveCubic(float a, float b, float c)
{
    float p = b - a*a / 3.0, p3 = p*p*p;
    float q = a * (2.0*a*a - 9.0*b) / 27.0 + c;
    float d = q*q + 4.0*p3 / 27.0;
    float offset = -a / 3.0;
    if(d >= 0.0)
    { 
        float z = sqrt(d);
        vec2 x = (vec2(z, -z) - q) / 2.0;
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        return vec3(offset + uv.x + uv.y);
    }
    float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
    float m = cos(v), n = sin(v)*1.732050808;
    return vec3(m + m, -n - m, n - m) * sqrt(-p / 3.0) + offset;
}

// How to resolve the equation below can be seen on this image.
// http://www.perbloksgaard.dk/research/DistanceToQuadraticBezier.jpg
vec3 getVectorToQuadraticBezier(vec2 p, vec2 p0, vec2 p1, vec2 p2) 
{
	p1 = mix(p1 + vec2(1e-4), p1, abs(sign(p1 * 2.0 - p0 - p2)));
	vec2 A = p1 - p0;
	vec2 B = p2 - p1 - A;
	vec2 C = p - p0;
	vec3 r = vec3(-3.*dot(A,B), dot(C,B)-2.*dot(A,A), dot(C,A)) / -dot(B,B);
	r = clamp(solveCubic(r.x, r.y, r.z), 0.0, 1.0);
	vec2 D = A * 2.0;
    
    float fs[3];
    fs[0] = r.x;
    fs[1] = r.y;
    fs[2] = r.z;
    
    float minDist = 10000.0;
    vec2 resultVector;
    
    for(int i=0; i<3; ++i) {
        
        vec2 vec = (p0+(D+B*fs[i])*fs[i]) - p;
        float d = length(vec);
        if(d < minDist) {
        	minDist = d;
            resultVector = vec;
        }
    }
    
    return vec3(resultVector / minDist, minDist);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p0 = vec2(-cos(iGlobalTime*0.5)*0.4,sin(iGlobalTime*0.9)*0.4);
	vec2 p1 = vec2(-cos(iGlobalTime*2.3),sin(iGlobalTime*1.1));
	vec2 p2 = vec2(cos(iGlobalTime*1.3)*0.4,-sin(iGlobalTime*0.3)*0.3);
	float zRot = sin(iGlobalTime)-iGlobalTime;
	float fCos = cos(zRot);
	float fSin = sin(zRot);
	p0 *= mat2(fCos,fSin,-fSin,fCos);
	zRot = sin(iGlobalTime*0.7);
	fCos = cos(zRot);
	fSin = sin(zRot);
	p1 *= mat2(fCos,fSin,-fSin,fCos);
	zRot = cos(iGlobalTime*1.8)+iGlobalTime;
	fCos = cos(zRot);
	fSin = sin(zRot);
	p2 *= mat2(fCos,fSin,-fSin,fCos);
	p0 += 1.0;
	p1 += 1.0;
	p2 += 1.0;
	p0 *= iResolution.xy*0.5;
	p1 *= iResolution.xy*0.5;
	p2 *= iResolution.xy*0.5;
    
    vec3 vec = getVectorToQuadraticBezier(fragCoord.xy,p0,p1,p2);
    if(vec.z > 50.)
        fragColor = vec4(0,0,0,1);
    else {
        float z = 1.0 - vec.z / 50.0;
        float factor = sqrt(1.0 - z * z);
        
    	fragColor = vec4(.5 + .5 * factor * vec.xy, z, 1);
    }
}
