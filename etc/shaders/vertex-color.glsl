
#version 300 es

precision mediump float;

in vec3 position;
in vec3 normal;
in vec3 color;

uniform mat4 world;
uniform mat4 mvp;
uniform mat4 depth_mvp;
uniform mat4 model_view;
uniform mat4 normal_matrix;
uniform vec3 camera_position;
uniform vec3 light_dir;
uniform vec3 light_intensity;

flat out float v_light;
flat out vec3 v_color;
flat out vec3 v_normal;
out vec3 v_ray;
out vec4 shadow_coord;

#include lighting.glsl

void main()
{
    vec4 v4_pos = vec4(position, 1.0);
    gl_Position = mvp * v4_pos;
    shadow_coord = depth_mvp * v4_pos;

    v_color = standard_light(color);
    // v_normal = trans_normal.xyz;
    v_ray = camera_position - (world * v4_pos).xyz;
}
