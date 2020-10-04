
#include profile

in vec3 position;
in vec3 normal;

#include uniforms.glsl

uniform bool is_white;

out shader_data {
#include shadervars.glsl
} data_out;

void main()
{
  vec3 color = is_white ? vec3(0.9, 0.9, 0.9) : vec3(0.3, 0.3, 0.3);
#include standard_vtxos.glsl
  gl_Position = mvp * v4_pos;
}
