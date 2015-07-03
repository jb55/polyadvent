#version 110

attribute vec3 position;
attribute vec3 normal;

uniform mat4 mvp;
uniform mat4 normal_matrix;

varying float v_dot;
uniform vec3 light_dir;

void main()
{
    vec4 trans_normal = normal_matrix * vec4(normal, 1);
    gl_Position = mvp * vec4(position.xyz / 10., 1.0);
    v_dot = max(dot(trans_normal.xyz, light_dir), 0.4);
}
