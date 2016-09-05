#version 130

varying vec2 texPosition;

void main() {
    gl_Position = ftransform();
	texPosition = gl_MultiTexCoord0.xy;
}
