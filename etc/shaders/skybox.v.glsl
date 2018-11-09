#include profile

in vec3 position;
in vec3 tex_coord;
out vec3 tex_coords;

uniform mat4 mvp;

void main() {
    tex_coords = position;
    vec4 pos = mvp * vec4(position.xyz, 0.01);
    gl_Position = pos;
}
