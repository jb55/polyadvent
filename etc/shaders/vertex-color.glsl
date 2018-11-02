#include profile
precision mediump float;
in vec3 position;
in vec3 normal;
in vec3 color;

#include uniforms.glsl

out f_data {
#include shadervars.glsl
} vertex;

flat out vec3 v_color;

void main()
{
#include standard_vtxos.glsl
}
