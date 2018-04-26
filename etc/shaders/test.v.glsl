#version 320 es

in vec3 position;
in vec3 normal;

uniform mat4 local;
uniform mat4 mvp;
uniform mat4 normal_matrix;

flat out float v_dot;
flat out vec3 v_norm;

uniform vec3 light_dir;

void main()
{
    vec4 trans_normal = normal_matrix * vec4(normal, 1);
    gl_Position = mvp * local * vec4(position.xyz, 1.0);
    v_dot = dot(trans_normal, vec4(light_dir, 0));
    v_norm = normal;
}
