#version 130

uniform vec3 uFaceNormal;

varying vec2 vPosition;
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec4 vColor;

void main() {

    vec3 up = vec3(0,1,0);
    vec3 u = cross(up, uFaceNormal);
    vec3 v = cross(uFaceNormal, u);

    vec3 normal = gl_Normal.xyz;
    float dot_u = dot(u, normal);
    float dot_v = dot(v, normal);

    vec4 vec = gl_Vertex;
    vec.xyz += 100.0 * (dot_u * u + dot_v * v);
    gl_Position = gl_ModelViewProjectionMatrix * (vec); 

    //gl_Position = ftransform();
	vPosition = gl_Vertex.xy;
	vTexCoord = gl_MultiTexCoord0.xy;
    vNormal = gl_Normal.xyz;
    vColor = gl_Color;
}
