#version 130

uniform int uShadowMode;

varying vec2 vModelPosition;
varying vec2 vScenePosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

void main() {

    vNormal = gl_NormalMatrix * gl_Normal;

    vModelPosition = gl_Vertex.xy;

    vec4 vertex = gl_Vertex;
    if(uShadowMode == 1)
        vertex += vec4(gl_Normal.xy * 3, 0, 0);

    vec4 locVert = gl_ModelViewMatrix * gl_Vertex;
	vScenePosition = (locVert.xyz / locVert.w).xy;
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vertex;

    //vNormal = gl_Normal.xyz;
	vTexCoord = gl_MultiTexCoord0.xy;
}
