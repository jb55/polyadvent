#include profile

out vec4 frag_color;

#include uniforms.glsl

in shader_data {
#include shadervars.glsl
} vertex;

uniform sampler2D shadow_map;

#include lighting.glsl
#include shadows.glsl
#include fog.glsl

void main() {
  vec3 v_ray = camera_position - vertex.frag_pos;
  vec4 v4_pos = vec4(vertex.position, 1.0);
  vec4 v4_normal = vec4(vertex.normal, 1.0);

  // vec3 color = vertex.color * (1.0-smoothness)
  //     + color_smooth * smoothness;
  vec3 color = standard_light(vertex.color, v4_pos, v4_normal);

  if (fog_on) {
    vec3 fog = apply_fog(color, length(v_ray), camera_position, v_ray);
    // vec3 fog = depth_fog(color, shadow_sample);
    color = fog;
  }

  color *= shadow_strength(v4_pos, v4_normal, vertex.shadow_coord);

  frag_color = vec4(gamma_correct(color), 1.0);
}
