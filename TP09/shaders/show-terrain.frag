#version 300 es

uniform sampler2D terrain;

in vec2 coord;

out vec4 outBuffer;

void main() {
  outBuffer = texture(terrain,coord);
}
