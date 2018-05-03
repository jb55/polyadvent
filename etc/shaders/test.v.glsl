#version 320 es

precision mediump float;

in vec3 position;
in vec3 normal;

uniform mat4 world;
uniform mat4 mvp;
uniform mat4 model_view;
uniform mat4 normal_matrix;
uniform vec3 camera_position;
uniform vec3 light_dir;

flat out float v_light;
flat out vec4 v_color;
out vec3 v_ray;


void main()
{
    vec4 trans_normal = normal_matrix * vec4(normal, 1);
    vec4 v4pos = vec4(position, 1.0);
    gl_Position = mvp * v4pos;
    v_light = dot(trans_normal, vec4(normalize(light_dir), 0));

    if (position.z <= 1.0)
      v_color = vec4(0.0, 0.5, 0.79, 1.0);
    else if (position.z <= 2.0)
      v_color = vec4(0.9176, 0.8156, 0.6588, 1.0);
    else if (position.z <= 5.0)
      v_color = vec4(0.6274, 0.749, 0.156, 1.0);
    else if (position.z <= 100.0)
      v_color = vec4(0.5, 0.5, 0.5, 1.0);
    else
      v_color = vec4(1.0, 1.0, 1.0, 1.0);

    v_ray = camera_position - (world * v4pos).xyz;
}
