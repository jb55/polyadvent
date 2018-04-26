#version 320 es


precision mediump float;

uniform float fade_factor;
flat in vec3 v_norm;
flat in float v_dot;
flat in vec4 v_hcol;
out vec4 fragmentColor;


void main() {
  fragmentColor = vec4(v_hcol * v_dot);
}
