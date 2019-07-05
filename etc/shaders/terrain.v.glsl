#include profile

in vec3 position;
in vec3 normal;

#include uniforms.glsl

out shader_data {

#include shadervars.glsl

} data_out;

void main()
{
    const int nlands = 6;

    // const vec4 land[nlands] = vec4[](
    //   vec4(0.9176, 0.8156, 0.6588, 1.0),  // 1 - sand
    //   vec4(0.4627, 0.3333, 0.1686, 20.0)  // 3 - dirt
    // );

    const vec4 land[nlands] = vec4[](
      vec4(0.0, 0.5, 0.79, 0.0),          // 0 - water
      vec4(0.9176, 0.8156, 0.6588, 1.0),  // 1 - sand
      vec4(0.7, 0.7, 0.156, 2.0),    // 2 - grass
      vec4(0.4627, 0.3333, 0.1686, 20.0), // 3 - dirt
      vec4(0.5, 0.5, 0.5, 80.0),         // 4 - stone
      vec4(1.0, 1.0, 1.0, 380.0)          // 5 - snow
    );

    vec3 color = land[0].xyz;
    for (int i = 0; i < nlands-1; i++) {
		color =
			mix(color,
			    land[i+1].xyz,
			    smoothstep(land[i].w, land[i+1].w, position.z));
	}

	// vec3 color = vec3(position.z*0.05, position.z*0.0055, position.z*0.0001) * 0.5;


     // vec3 color = vec3(position.z, position.z, position.z) * 0.005;

#include standard_vtxos.glsl

    gl_Position = mvp * v4_pos;
}
