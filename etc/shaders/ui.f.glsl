#include profile

precision mediump float;

in vec3 v_color;
in vec2 v_tex_coords;

out vec4 fragmentColor;

uniform sampler2D screen_texture;

void main() {
    // fragmentColor = vec4(color + diffuse, 1.0);
    fragmentColor = texture(screen_texture, v_tex_coords) + vec4(v_color, 0.0) * 0.0001;
}
