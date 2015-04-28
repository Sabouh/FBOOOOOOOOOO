#version 330

out vec4 bufferColor;

// TODO: get input textures and light direction as uniform variables 
uniform vec4 norm;
uniform vec3 light;
uniform sampler2D inputmap;


void main() {
  // TODO: compute lighting using input map information
  //bufferColor = vec4(1,0,0,1);
  bufferColor = texelFetch(inputmap,ivec2(gl_FragCoord.xy),0)*vec4(light,1.0);
}
