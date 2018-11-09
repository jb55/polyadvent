#include profile

in vec3 position;
out vec3 tex_coords;

uniform mat4 mvp;

void main() {
    tex_coords = position;
    vec4 pos = mvp * vec4(position.xyz, 1.0);
    gl_Position = pos.xyww;
}
