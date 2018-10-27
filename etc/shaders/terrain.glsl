#version 300 es

precision mediump float;

in vec3 position;
in vec3 normal;

uniform mat4 world;
uniform mat4 mvp;
uniform mat4 model_view;
uniform mat4 normal_matrix;
uniform vec3 camera_position;
uniform vec3 light_dir;
uniform vec3 light_intensity;

flat out float v_light;
flat out vec3 v_color;
out vec3 v_ray;

const int nlands = 6;

vec3 standard_light(vec3 color) {
	vec4 v4_normal = vec4(normal , 1);
	vec4 trans_normal = normal_matrix * v4_normal;

	float light = dot(trans_normal.xyz, normalize(light_dir)) ;
    return color * light;
}

vec3 hemispherical(vec3 color) {
	vec4 v4_normal = vec4(normal , 1);
	vec4 trans_normal = normal_matrix * v4_normal;

    vec3 L = light_dir;
    vec3 N = normalize(trans_normal.xyz);

    float costheta = dot(L,N);

    float a = 0.5 + (0.5 * costheta);
    return a * light_intensity * color
   + (1.0-a) * vec3(0.0, 0.0, 0.0) * color;
}

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}

const vec4 land[nlands] = vec4[](
  vec4(0.0, 0.5, 0.79, 0.0),          // 0 - water
  vec4(0.9176, 0.8156, 0.6588, 1.0),  // 1 - sand
  vec4(0.6274, 0.749, 0.156, 2.0),    // 2 - grass
  vec4(0.4627, 0.3333, 0.1686, 20.0), // 3 - dirt
  vec4(0.5, 0.5, 0.5, 80.0),         // 4 - stone
  vec4(1.0, 1.0, 1.0, 380.0)          // 5 - snow
);

void main()
{
	vec4 v4_pos = vec4(position, 1.0);
	gl_Position = mvp * v4_pos;

    vec3 color = land[0].xyz;

	for (int i = 0; i < nlands-1; i++) {
		color =
			mix(color,
			    land[i+1].xyz,
			    smoothstep(land[i].w, land[i+1].w, position.z));
	}

	// v_color = vec3(0.2 + position.z*0.05, position.z*0.0095, position.z*0.0001) * 0.5;
	 // v_color = vec3(position.z, position.z, position.z) * 0.005;

    v_color = hemispherical(color);
	v_ray = camera_position - (world * v4_pos).xyz;
}
