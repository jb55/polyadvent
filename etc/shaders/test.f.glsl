#version 300 es

precision mediump float;

flat in vec3 v_color;
in vec3 v_ray;
out vec4 fragmentColor;

uniform vec3 camera_position;
uniform bool fog_on;
uniform bool diffuse_on;
uniform vec3 light_intensity;
uniform mat4 normal_matrix;

vec3 apply_fog(in vec3 rgb, in float distance, in vec3 ray_orig, in vec3 ray_dir) {
  const float b = 0.00026;
  const float v = 1.0;
  const float zs = 1.0;


  const float c = 1.0;

  // float fog_amount =
  //   exp(-ray_orig.z*b) * (1.0-exp( -distance*ray_dir.z*b))/ray_dir.z*b;

  float fog_amount = 1.0 - exp(-(pow(distance*b, 2.0))) ;

  vec3  fog_color  = vec3(0.5,0.6,0.7) * light_intensity;
  return mix( rgb, fog_color, fog_amount);
}


void main() {
  vec3 light_position = vec3(2.0,1.0,5.0);

  vec3 color;

  vec3 frag = v_color;

  if (fog_on) {
    vec3 fog = apply_fog(frag, length(v_ray), camera_position, v_ray);
    color = fog;
  }
  else
    color = frag;

  // fragmentColor = vec4(color + diffuse, 1.0);
  fragmentColor = vec4(color, 1.0);
}
