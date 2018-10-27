#version 300 es

precision mediump float;

in vec3 v_color;
out vec4 fragmentColor;

void main() {
    // fragmentColor = vec4(color + diffuse, 1.0);
    fragmentColor = vec4(v_color, 1.0);
}
