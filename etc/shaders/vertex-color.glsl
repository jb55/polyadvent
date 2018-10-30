
#version 300 es

precision mediump float;

in vec3 position;
in vec3 normal;
in vec3 color;

#include uniforms.glsl

flat out float v_light;
flat out vec3 v_color;
out vec3 v_color_smooth;
out vec3 v_normal;
out vec3 v_position;
out vec3 v_ray;
out vec4 shadow_coord;

#include lighting.glsl

void main()
{
#include standard_vtxos.glsl
}
