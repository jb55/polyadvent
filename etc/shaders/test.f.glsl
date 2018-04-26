#version 320 es


precision mediump float;

uniform float fade_factor;
flat in vec3 v_norm;
flat in float v_dot;
out vec4 fragmentColor;


void main() {
  vec4 color = vec4(0.4, 0.4, 0.8, 1.0);
  fragmentColor = vec4(color.xyz * v_dot, 1.0);
}
