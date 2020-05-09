
#include profile

in vec3 position;
in vec3 normal;

#include uniforms.glsl

out shader_data {
#include shadervars.glsl
} data_out;

void main()
{
  vec3 color = piece_color;
#include standard_vtxos.glsl
  gl_Position = mvp * v4_pos;
}
