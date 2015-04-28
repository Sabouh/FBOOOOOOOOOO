#version 300 es

layout(location = 0) out vec4 outBufferNormal;
layout(location = 1) out vec4 outBufferCouleur;
layout(location = 2) out vec4 outBufferLumiere;

uniform sampler2D coul;
//uniform sampler2D shadowMap;
uniform sampler2D eauTex;
uniform sampler2D foretTex;
uniform sampler2D rocheTex;
uniform sampler2D neigeTex;

in vec2 coord;
in vec3 normal;
in vec3 p;
in  vec3 normalView;

in vec4 fragmentNormal;
in vec4 fragmentLumiere;

void main() {
	
         outBufferCouleur = texture(coul,coord);  
         outBufferLumiere = fragmentLumiere;
         outBufferNormal = fragmentNormal;
}
