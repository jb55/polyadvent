#include profile

precision mediump float;

out vec4 frag_color;

#include uniforms.glsl

in f_data {
#include shadervars.glsl
} vertex;

// in flat vec3 v_color;
// in vec3 v_color_smooth;
// in vec3 v_ray;
// in vec3 v_normal;
// in vec3 v_position;
// in vec4 v_shadow_coord;
// in vec3 v_frag_pos;

uniform sampler2D shadow_map;

#include lighting.glsl
#include fog.glsl

void main() {
  const float smoothness = 0.0;
  float visibility = 1.0;
  vec4 shadow_sample = texture(shadow_map, vertex.shadow_coord.xy);
  vec3 v_ray = camera_position - vertex.frag_pos;

  // vec3 color = vertex.color * (1.0-smoothness)
  //     + color_smooth * smoothness;
  vec3 color = standard_light(vertex.color, vertex.position, vertex.normal);

  if (fog_on) {
    vec3 fog = apply_fog(color, length(v_ray), camera_position, v_ray);
    // vec3 fog = depth_fog(color, shadow_sample);
    color = fog;
  }

  if (light_dir.z > 0.0 && shadow_sample.z < vertex.shadow_coord.z ) {
    float factor = 1.0/dot(light_dir, vec3(0.0, 0.0, 1.0));
    visibility = clamp(0.2 * factor, 0.5, 1.0);
  }

  color = color * visibility;
  // float dist = length(camera_position - vertex.position);

  frag_color = vec4(color, 1.0);
}
