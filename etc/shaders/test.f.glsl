#version 300 es

precision mediump float;

flat in vec3 v_color;
in vec3 v_color_smooth;
in vec3 v_ray;
in vec3 v_normal;
in vec3 v_position;
in vec4 shadow_coord;
in vec3 v_frag_pos;
out vec4 frag_color;

#include uniforms.glsl

uniform sampler2D shadow_map;

#include lighting.glsl
#include fog.glsl

void main() {

  const float smoothness = 0.0;
  float visibility = 1.0;
  vec4 shadow_sample = texture(shadow_map, shadow_coord.xy);
  vec3 v_ray = camera_position - v_frag_pos;

  // vec3 color = v_color * (1.0-smoothness)
  //     + v_color_smooth * smoothness;
  vec3 color = standard_light(v_color, v_position);

  if (fog_on) {
    vec3 fog = apply_fog(color, length(v_ray), camera_position, v_ray);
    // vec3 fog = depth_fog(color, shadow_sample);
    color = fog;
  }

  if (light_dir.z > 0.0 && shadow_sample.z < shadow_coord.z ) {
    float factor = 1.0/dot(light_dir, vec3(0.0, 0.0, 1.0));
    visibility = clamp(0.2 * factor, 0.5, 1.0);
  }

  color = color * visibility;
  // float dist = length(camera_position - v_position);

  // fragmentColor = vec4(color + diffuse, 1.0);
  frag_color = vec4(color, 1.0);
  // fragmentColor = vec4(color * 0.0001 + length(v_ray) * 0.0001, 1.0);
}
