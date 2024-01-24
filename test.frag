#version 410

in vec3 color;
uniform vec4 inputColor;
out vec4 frag_color;

void main () {
  // frag_color = vec4(color, 1.0);
  frag_color = inputColor;
}

