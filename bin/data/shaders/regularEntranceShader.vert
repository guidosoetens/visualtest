#version 130

varying vec2 vTexCoord;
varying vec2 vScenePosition;

void main() {
    gl_Position = ftransform();
	vTexCoord = gl_MultiTexCoord0.xy;

    vec4 locVert = gl_ModelViewMatrix * gl_Vertex;
	vScenePosition = (locVert.xyz / locVert.w).xy;
}
