#include profile
precision mediump float;
in vec3 position;
in vec3 normal;
in vec3 color;

out shader_data {
#include shadervars.glsl
} vertex;

#include uniforms.glsl


void main()
{
#include standard_vtxos.glsl

    gl_Position = mvp * v4_pos;
}
