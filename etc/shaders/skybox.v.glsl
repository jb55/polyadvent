#include profile

in vec3 position;
out vec3 tex_coords;

uniform mat4 mvp;

void main() {
    tex_coords = position;
    gl_Position = mvp * vec4(position, 1.0);
}
