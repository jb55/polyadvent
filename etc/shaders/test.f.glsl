
uniform float fade_factor;
varying vec3 v_norm;
out vec4 frag_color;

void main() {
  vec4 color = vec4(0.4, 0.4, 0.8, 0.5);
  frag_color = vec4(v_norm, 1);
}
