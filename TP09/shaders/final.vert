#version 300 es

// input attributes
layout(location = 0) in vec3 position;

uniform mat4 mdvMat;
uniform mat4 projMat;

uniform sampler2D terrain;

uniform vec3 light;

out vec2 coord;


void main() {
  coord = (position.xy*0.5+0.5);
  gl_Position = vec4(position,1.0);  
}

