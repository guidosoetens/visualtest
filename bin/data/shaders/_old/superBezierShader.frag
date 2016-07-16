#version 130

precision mediump float;

varying vec2 vPosition;
varying vec2 vTexCoord;

uniform vec2 uResolution;
uniform float uTimeParameter;
uniform float uNetworkOffset;

uniform vec2 uAnchor1;
uniform vec2 uAnchor2;
uniform vec2 uController;

const float pi = 3.1415926535;

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
        
        if(x.x == 0.0) {
            x.x = .000000001;
        }
        
        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
        return vec3(offset + uv.x + uv.y);
    }
    float v = acos(-sqrt(-27.0 / p3) * q / 2.0) / 3.0;
    float m = cos(v), n = sin(v)*1.732050808;
    return vec3(m + m, -n - m, n - m) * sqrt(-p / 3.0) + offset;
} 

vec3 getVectorToQuadraticBezier(vec2 p) 
{
    /*
    vec2 p0 = uAnchor1;
    vec2 p1 = uController;
    vec2 p2 = uAnchor2;
    */
    
    vec2 p0 = vec2(1,0);
    vec2 p1 = vec2(0,0);
    vec2 p2 = vec2(0,1);
    
	vec2 A = p1 - p0;
	vec2 B = p2 - p1 - A;
	vec2 C = p - p0;
    
    
	vec3 r = vec3(-3.*dot(A,B), dot(C,B)-2.*dot(A,A), dot(C,A)) / -dot(B,B);
	r = solveCubic(r.x, r.y, r.z);
	vec2 D = A * 2.0;
    
    float fs[3];
    fs[0] = r.x;
    fs[1] = r.y;
    fs[2] = r.z;
    
    float minDistSqrd = 1.0 / 0.0;
    vec2 resultVector;
  
    for(int i=0; i<3; ++i) {
        
        float t = fs[i];
        if(t < -.1)
            continue;
            
        t = clamp(t, 0.0, 1.0);
      
        vec2 vec = (p0+(D+B*t)*t) - p;
        float dSqrd = dot(vec, vec);
        if(dSqrd < minDistSqrd) {
            minDistSqrd = dSqrd;
            resultVector = vec;
        }
    }
    
    vec2 perp = p2 - p0;
    perp = vec2(-perp.y, perp.x);
 
    float minDist = sqrt(minDistSqrd);
    vec3 result = vec3(resultVector / minDist, minDist);
    
    if(dot(perp, result.xy) < 0.0)
        result.z = -result.z;
    
    return result;
}

void main()
{
    float offset = 1.0 * uNetworkOffset;
    
    gl_FragColor = vec4(vTexCoord, 0, 1);
   // return;
    
    vec3 vec = getVectorToQuadraticBezier(vTexCoord);//vPosition.xy);
    
    vec2 goalPosBary = vTexCoord + vec.xy * abs(vec.z);
    
    //barycentric to regular...
    vec2 goalPos = uController + goalPosBary.x * (uAnchor1 - uController) + goalPosBary.y * (uAnchor2 - uController);
    
    vec2 to  = goalPos - vPosition;
    float dist = length(to);
    vec = vec3(to / dist, sign(vec.z) * dist);
    
    
    
    if(vec.z > offset)
        gl_FragColor = vec4(0,0,0,0);
    else {
        float z = 1.0 - clamp(vec.z, 0, offset) / offset;
        float factor = sqrt(1.0 - z * z);
    	gl_FragColor = vec4(.5 + .5 * factor * vec.xy, z, 1);
        
    }
}
