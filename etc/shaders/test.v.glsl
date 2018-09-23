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
flat out vec3 v_color;
flat out vec3 v_normal;
out vec3 v_ray;


void main()
{
	vec4 v4_normal = vec4(normal, 1);
	vec4 trans_normal = normal_matrix * v4_normal;
	vec4 v4_pos = vec4(position, 1.0);
	gl_Position = mvp * v4_pos;
	v_light = dot(trans_normal, vec4(normalize(light_dir), 0));

	// if (position.z <= 1.0)
	// 	v_color = vec3(0.0, 0.5, 0.79);
	// else if (position.z <= 2.0)
	// 	v_color = vec3(0.9176, 0.8156, 0.6588);
	// else if (position.z <= 20.0)
	// 	v_color = vec3(0.6274, 0.749, 0.156);
	// else if (position.z <= 60.0)
	// 	v_color = vec3(0.8784, 0.8784, 0.7);
	// else if (position.z <= 100.0)
	// 	v_color = vec3(0.5, 0.5, 0.5)9
	// else
	// 	v_color = vec3(1.0, 1.0, 1.0) * 0.9;

	v_color = vec3(position.z*0.05, position.z*0.005, position.z*0.0001) * 0.5;

	v_normal = trans_normal.xyz;
	v_ray = camera_position - (world * v4_pos).xyz;
}
