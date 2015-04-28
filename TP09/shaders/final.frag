#version 300 es

out vec4 outBuffer;

uniform sampler2D normal;
uniform sampler2D couleur;
uniform sampler2D lumiere;
uniform sampler2D terrain;

in vec2 coord;


void main() {
	outBuffer = texture(normal,coord);
    //outBuffer = texture(couleur,coord)*texture(normal,coord)*texture(lumiere,coord);
}
