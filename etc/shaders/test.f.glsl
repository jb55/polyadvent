#version 320 es

precision mediump float;

flat in float v_light;
flat in vec4 v_color;
in vec3 v_ray;
out vec4 fragmentColor;

uniform vec3 camera_position;

vec3 apply_fog(in vec3 rgb, in float distance, in vec3 ray_orig, in vec3 ray_dir) {
  const float b = 0.0000032;
  const float v = 1.0;
  const float zs = 1.0;
  
  
  const float c = 1.0;

  float fog_amount =
    exp(-ray_orig.z*b*zs) * (1.0-exp( -distance*ray_dir.z*b*zs ))/ray_dir.z*b;

  // float fog_amount = 1.0 - exp(-distance*0.0000032);

  vec3  fog_color  = vec3(0.5,0.6,0.7);
  return mix( rgb, fog_color, fog_amount * 100.0 );
}

void main() {
  float distance = length(v_ray);
  
  // if (distance > 1000.0)
  //   fragmentColor = vec4(1.0, 0.0, 0.0, 0.0);
  // else {

  // fragmentColor = (vec4(v_ray, 0.0) * 0.001 - v_color) * v_light;
  
  // fragmentColor = v_color * (1.0 - distance*0.0001) * v_light;
  
  vec3 fog = apply_fog((v_color * v_light).xyz, distance, camera_position, -v_ray);
  fragmentColor = vec4(fog, 0.0);
  // fragmentColor = vec4((v_color * v_light).xyz, 0.0);
  // }
}
