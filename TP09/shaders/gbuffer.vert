#version 300 es

// input attributes
layout(location = 0) in vec3 position;

uniform mat4 mdvMat;
uniform mat4 projMat;
uniform mat4 normalMat;

uniform sampler2D terrain;
uniform sampler2D coul;

uniform vec3 light;

out vec2 coord;
out vec3 normal;
out vec3 p;
out vec3 normalView;

out vec4 fragmentNormal;
out vec4 fragmentLumiere;


vec3 calculNormal(){
	float alpha = 100;
	
	float ps = 1./512;
	float hh = texture(terrain,coord+vec2(0,ps)).x;
	float hb = texture(terrain,coord-vec2(0,ps)).x;
	float hd = texture(terrain,coord+vec2(ps,0)).x;
	float hg = texture(terrain,coord-vec2(ps,0)).x;
	
	float gx = (hd-hg)*alpha;
	float gy = (hh-hb)*alpha;
	
	vec3 v1 = vec3(1,0,gx);
	vec3 v2 = vec3(0,1,gy);
	
	return normalize(cross(v1,v2));
}

void main() {
  p = position;  
  coord = position.xy*0.5+0.5;
  p.z = texture(terrain,coord).x; 
  //p = p + vec3(0,0,1) * texture(terrain, coord).xyz;
  gl_Position = projMat*mdvMat*vec4(p,1.0);  
  normal = calculNormal();
  //lumiere
 // float diff = max(dot(light,normal),0.0);
  fragmentLumiere = (texture(coul,coord));
  //normal
  fragmentNormal = vec4(normal,1.0);
  
//  normalView  = normalize(normalMat*normal);
}

