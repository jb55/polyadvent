#include profile

out vec4 frag_color;

#include uniforms.glsl

in shader_data {
#include shadervars.glsl
} vertex;

uniform sampler2D shadow_map;
uniform samplerCube skybox;

#include lighting.glsl
#include shadows.glsl
#include fog.glsl

vec3 reflect_env(vec3 vert_pos) {
    vec3 I = normalize(vert_pos - camera_position);
    vec3 R = reflect(I, normalize(vertex.normal));
    vec3 color = texture(skybox, R).rgb;
    return color;
}

vec3 refract_env(vec3 vert_pos) {
    const float ratio = 1.00 / 1.52;

    vec3 I = normalize(vert_pos - camera_position);
    vec3 R = refract(I, normalize(vertex.normal), ratio);
    vec3 color = texture(skybox, R).rgb;
    return color;
}

void main() {
  vec3 V = camera_position - vertex.frag_pos;
  vec4 v4_pos = vec4(vertex.position, 1.0);
  vec4 v4_normal = vec4(vertex.normal, 1.0);

  // vec3 color = standard_light(vertex.color, v4_pos, v4_normal);
  vec3 color = pbr(vertex.color, normalize(V), vertex.normal);

  if (fog_on) {
    vec3 fog = apply_fog(color, length(V), camera_position, V);
    color = fog;
  }

  color *= shadow_strength(v4_pos, v4_normal, vertex.shadow_coord);
  // vec3 color = reflect_env(vertex.position);

  frag_color = vec4(gamma_correct(color), 1.0);
}
