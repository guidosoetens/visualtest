#version 130

uniform float uRotation;

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
    gl_Position = ftransform();

	vScenePosition = gl_Position.xy;
    vModelPosition = gl_Vertex.xy;
    vNormal = gl_Normal.xyz;
    vNormal.xy = rotate2D(vNormal.xy, uRotation);
}