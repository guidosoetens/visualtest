#version 130

uniform int uShadowMode;

varying vec2 vTexCoord;
varying vec3 vNormal;

void main() {
    vec4 vertex = gl_Vertex;
    if(uShadowMode == 1)
        vertex += vec4(gl_Normal.xy * 3, 0, 0);
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vertex;

    vNormal = gl_Normal.xyz;
	vTexCoord = gl_MultiTexCoord0.xy;
}
