
#version 300 es

precision mediump float;

in vec3 position;
in vec3 normal;
in vec3 color;

uniform mat4 world;
uniform mat4 mvp;
uniform mat4 depth_mvp;
uniform mat4 model_view;
uniform mat4 normal_matrix;
uniform vec3 camera_position;
uniform vec3 light_dir;
uniform vec3 light_intensity;

flat out float v_light;
flat out vec3 v_color;
flat out vec3 v_normal;
out vec3 v_ray;
out vec4 shadow_coord;

// TODO: includes
// #include "lighting.glsl"



vec3 standard_light(vec3 color) {
	vec4 v4_normal = vec4(normal , 1);
	vec4 trans_normal = normal_matrix * v4_normal;

    vec3 L = light_dir;
    vec3 N = normalize(trans_normal.xyz);

    float costheta = clamp(dot(L,N), 0.1, 1.0);

    return color * costheta;
}

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}


void main()
{
    vec4 v4_pos = vec4(position, 1.0);
    gl_Position = mvp * v4_pos;
    shadow_coord = depth_mvp * v4_pos;

    v_color = standard_light(color);
    // v_normal = trans_normal.xyz;
    v_ray = camera_position - (world * v4_pos).xyz;
}
