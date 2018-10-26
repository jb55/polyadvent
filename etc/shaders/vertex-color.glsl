
#version 300 es

precision mediump float;

in vec3 position;
in vec3 normal;
in vec3 color;

uniform mat4 world;
uniform mat4 mvp;
uniform mat4 model_view;
uniform mat4 normal_matrix;
uniform vec3 camera_position;
uniform vec3 light_dir;
uniform vec3 light_intensity;

flat out float v_light;
flat out vec3 v_color;
flat out vec3 v_normal;
out vec3 v_ray;


vec3 standard_light(vec3 color) {
    vec4 v4_normal = vec4(normal, 1);
    vec4 trans_normal = normal_matrix * v4_normal;

	float light = dot(trans_normal.xyz, normalize(light_dir));
    return color * light;
}

vec3 hemispherical(vec3 color) {
    vec4 v4_normal = vec4(normal, 1);
    vec4 trans_normal = normal_matrix * v4_normal;

    vec3 L = normalize(light_dir);
    vec3 N = normalize(trans_normal.xyz);

    float costheta = dot(L,N);

    float a = 0.5 + (0.5 * costheta);
    return a * light_intensity * color
        + (1.0-a) * vec3(0.0, 0.0, 0.0) * color;
}

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}


void main()
{
    vec4 v4_pos = vec4(position, 1.0);
    gl_Position = mvp * v4_pos;

    v_color = hemispherical(color);
    // v_normal = trans_normal.xyz;
    v_ray = camera_position - (world * v4_pos).xyz;
}
