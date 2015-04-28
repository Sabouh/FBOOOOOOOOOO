#version 300 es

out vec4 outBuffer;

uniform sampler2D terrain;

in vec2 coord;
in vec3 normal;
in vec4 fragmentColor;

void main() {
	outBuffer = fragmentColor;
}
