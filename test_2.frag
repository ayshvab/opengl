#version 410

in vec3 color;
out vec4 frag_color;

void main () {
  frag_color = vec4(color*0.2, 1.0);
};
