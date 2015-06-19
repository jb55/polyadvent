uniform float fade_factor;

void main() {
  gl_FragColor = vec4(0.4, 0.4, 0.8, 1.0) * fade_factor;
}