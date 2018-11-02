#include profile

precision mediump float;

in vec3 position;
in vec3 normal;

#include uniforms.glsl

layout(location = 10) out v_data {
#include shadervars.glsl
} vertices;


void main()
{
    const int nlands = 6;


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

	// vec3 color = vec3(position.z*0.05, position.z*0.0095, position.z*0.0001) * 0.5;


     // v_color = vec3(position.z, position.z, position.z) * 0.005;

// #include standard_vtxos.glsl
    vec4 v4_pos = vec4(position, 1.0);
    vertices.normal = normal;
    vertices.color_smooth = vertices.color = color;
    vertices.shadow_coord = depth_mvp * v4_pos;
    vertices.position = v4_pos.xyz;
    vertices.frag_pos = (world * v4_pos).xyz;

    gl_Position = mvp * v4_pos;
    vertices.position = gl_Position.xyz;
}
