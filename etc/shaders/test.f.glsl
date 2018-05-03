#version 320 es

precision mediump float;

flat in float v_light;
flat in vec4 v_color;
in vec3 v_ray;
out vec4 fragmentColor;

uniform vec3 camera_position;
uniform bool fog_on;
uniform mat4 normal_matrix;

vec3 apply_fog(in vec3 rgb, in float distance, in vec3 ray_orig, in vec3 ray_dir) {
  const float b = 0.00046;
  const float v = 1.0;
  const float zs = 1.0;


  const float c = 1.0;

  // float fog_amount =
  //   exp(-ray_orig.z*b) * (1.0-exp( -distance*ray_dir.z*b))/ray_dir.z*b;

  float fog_amount = 1.0 - exp(-(pow(distance*b, 2.0))) ;

  vec3  fog_color  = vec3(0.5,0.6,0.7);
  return mix( rgb, fog_color, fog_amount);
}

vec3 gamma_correct(vec3 color) {
  return pow(color, vec3(1.0/2.2));
}

void main() {
  float distance = length(v_ray);

  // if (distance > 1000.0)
  //   fragmentColor = vec4(1.0, 0.0, 0.0, 0.0);
  // else {

  // fragmentColor = (vec4(v_ray, 0.0) * 0.001 - v_color) * v_light;

  // fragmentColor = v_color * (1.0 - distance*0.0001) * v_light;

 // vec3 diffuse = dot()

  vec3 color;
  vec3 frag = (v_color * v_light).xyz;
  if (fog_on) {
    vec3 fog = apply_fog(frag, distance, camera_position, v_ray);
    color = fog;
  }
  else
    color = frag;
  // fragmentColor = vec4(fog, 1.0);
  // vec3 color = (v_color * v_light).xyz;
  fragmentColor = vec4(color, 1.0);
  // }
}
