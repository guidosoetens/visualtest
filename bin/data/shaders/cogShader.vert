#version 130

uniform float uRotation;
uniform int uShadowMode;

varying vec2 vModelPosition;
varying vec2 vScenePosition;
varying vec3 vNormal;

vec2 rotate2D(vec2 vec, float angle) {
    float cs = cos(angle);
    float sn = sin(angle);
    return vec2(cs * vec.x - sn * vec.y, sn * vec.x + cs * vec.y);
}

void main(void)
{
    vec4 vertex = gl_Vertex;
    if(uShadowMode == 1)
        vertex += vec4(gl_Normal.xy * 3, 0, 0);
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vertex;

    vec4 locVert = gl_ModelViewMatrix * vertex;

	vScenePosition = (locVert.xyz / locVert.w).xy;
    vModelPosition = gl_Vertex.xy;
    vNormal = gl_Normal.xyz;
    vNormal.xy = rotate2D(vNormal.xy, uRotation);
}