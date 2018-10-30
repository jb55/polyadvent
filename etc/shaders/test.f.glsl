#version 300 es

precision mediump float;

flat in vec3 v_color;
in vec3 v_color_smooth;
in vec3 v_ray;
in vec3 v_normal;
in vec3 v_position;
in vec4 shadow_coord;
out vec4 fragmentColor;

uniform vec3 camera_position;
uniform bool fog_on;
uniform bool diffuse_on;
uniform float light_intensity;
uniform vec3 light_dir;
uniform mat4 normal_matrix;

uniform sampler2D shadow_map;

vec3 apply_fog(in vec3 rgb, in float distance, in vec3 ray_orig, in vec3 ray_dir) {
  const float b = 0.00035;

  float fog_amount = 1.0-exp(-(pow(distance * b, 3.0))) ;
  float sun_amount = max( dot( ray_dir, -light_dir ), 0.0 ) * b;

  vec3  fog_color  = mix(vec3(0.5,0.6,0.7),
                         vec3(1.0,0.9,0.8),
                         sun_amount
                         ) * light_intensity;

  // vec3 final_color = rgb*(1.0-exp(-distance*b)) + fog_color*exp(-distance*b);
  vec3 final_color = mix( rgb, fog_color, fog_amount);
  return final_color;
}

void main() {

  const float smoothness = 0.0;
  float visibility = 1.0;
  float shadow_map_z = texture(shadow_map, shadow_coord.xy).z;

  vec3 color = v_color * (1.0-smoothness)
      + v_color_smooth * smoothness;

  if (fog_on) {
    vec3 fog = apply_fog(color, length(v_ray), camera_position, v_ray);
    color = fog;
  }

  if (light_dir.z > 0.0 && shadow_map_z < shadow_coord.z ) {
    float factor = 1.0/dot(light_dir, vec3(0.0, 0.0, 1.0));
    visibility = clamp(0.2 * factor, 0.5, 1.0);
  }

  color = color * visibility;

  // fragmentColor = vec4(color + diffuse, 1.0);
  fragmentColor = vec4(color, 1.0);
}
