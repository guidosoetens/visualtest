#version 130

uniform int uShadowMode;

varying vec2 vScenePosition;
varying vec2 vTexCoord;
varying vec3 vNormal;

void main() {
    vec4 vertex = gl_Vertex;
    if(uShadowMode == 2) {
        vertex.xy *= 1.1;
        vertex.xy += vec2(-1. / 40.0, 2. / 40.0);
    }
        //vertex += vec4(-5 / 40.0, -5 / 40.0, 0, 0);//vec4(gl_Normal.xy * 8 / 40.0, 0, 0);
    if(uShadowMode == 1)
        vertex += vec4(normalize(gl_Normal.xy) * 3 / 40.0, 0, 0);

    vec4 locVert = gl_ModelViewMatrix * gl_Vertex;
	vScenePosition = (locVert.xyz / locVert.w).xy;
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vertex;

    vNormal = gl_Normal.xyz;
	vTexCoord = gl_MultiTexCoord0.xy;
}
