#include profile

layout(triangles, equal_spacing, cw) in;

in shader_data {
#include shadervars.glsl
} data_in[];

out shader_data {
#include shadervars.glsl
} data_out;

#include uniforms.glsl


// out vec3 tePatchDistance = gl_TessCoord;


void main()
{
    vec4 v = gl_TessCoord.x * vec4(data_in[0].position, 1.0);
    + gl_TessCoord.y * vec4(data_in[1].position, 1.0);
    + gl_TessCoord.z * vec4(data_in[2].position, 1.0);
    // tePatchDistance = gl_TessCoord;
    gl_Position = mvp * v;

    data_out.position = gl_Position.xyz;
    data_out.color = data_in[0].color;
    data_out.color_smooth = data_in[0].color_smooth;
    data_out.normal = data_in[0].normal;
    // data_out.shadow_coord = depth_mvp * data_in[0].shadow_coord;
    data_out.world_pos = (world * v).xyz;

}
