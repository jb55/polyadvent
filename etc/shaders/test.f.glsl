uniform float fade_factor;
varying float v_dot;

void main() {
  vec4 color = vec4(0.4, 0.4, 0.8, 1.0);
  gl_FragColor = vec4(color.xyz * v_dot, 1.0);
}
