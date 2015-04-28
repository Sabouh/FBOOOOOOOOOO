#version 330

in  vec3  normalView;
in  vec3  tangentView;
in  vec3  eyeView;
in  vec2  uvcoord;
in  float depth;

uniform vec3      color;
uniform sampler2D normalmap;

// TODO: modify nb of outputs 
out vec4 outBuffer;

vec3 getModifiedNormal() {
  vec3 n   = normalize(normalView);
  vec3 t   = normalize(tangentView);
  vec3 b   = normalize(cross(n,t));
  mat3 tbn = mat3(t,b,n);
  vec3 tn  = normalize(texture2D(normalmap,uvcoord).xyz*2.0-vec3(1.0));
  
  return normalize(tbn*tn);
}

void main() {
  vec3 n = getModifiedNormal();
  vec3 l = normalize(vec3(1,1,0)); // TMP light for simple rendering

  outBuffer = vec4(n,1);
}
