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

flat out float v_light;
flat out vec3 v_color;
flat out vec3 v_normal;
out vec3 v_ray;


void main()
{
	vec4 v4_normal = vec4(normal, 1);
	vec4 trans_normal = normal_matrix * v4_normal;
	vec4 v4_pos = vec4(position, 1.0);
	gl_Position = mvp * v4_pos;
	v_light = dot(trans_normal.xyz, normalize(light_dir));

	v_color = color;

	// v_normal = trans_normal.xyz;
	v_ray = camera_position - (world * v4_pos).xyz;
}
