#version 300 es

out vec4 outBuffer;

uniform sampler2D terrain;

in vec2 coord;

void main() {
  outBuffer = texture(terrain,coord);
}
