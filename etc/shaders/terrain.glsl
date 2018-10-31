#version 300 es

precision mediump float;

in vec3 position;
in vec3 normal;

#include uniforms.glsl

flat out float v_light;
flat out vec3 v_color;
out vec3 v_color_smooth;

out vec3 v_normal;
out vec3 v_ray;
out vec3 v_position;
out vec3 v_frag_pos;
out vec4 shadow_coord;

const int nlands = 6;

#include lighting.glsl


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
    vec3 color = land[0].xyz;
    for (int i = 0; i < nlands-1; i++) {
		color =
			mix(color,
			    land[i+1].xyz,
			    smoothstep(land[i].w, land[i+1].w, position.z));
	}

	// vec3 color = vec3(position.z*0.05, position.z*0.0095, position.z*0.0001) * 0.5;


     // v_color = vec3(position.z, position.z, position.z) * 0.005;

#include standard_vtxos.glsl
}
