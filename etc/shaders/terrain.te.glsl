#include profile

layout(triangles, equal_spacing, cw) in;

in shader_dataz {
#include shadervars.glsl
} tc_vertices[];

out shader_data {
#include shadervars.glsl
} vertex;

#include uniforms.glsl


// out vec3 tePatchDistance = gl_TessCoord;


void main()
{
    vertex.color = tc_vertices[0].color;
    vertex.color_smooth = tc_vertices[0].color_smooth;
    vertex.normal = tc_vertices[0].normal;
    vertex.shadow_coord = depth_mvp * tc_vertices[0].shadow_coord;

    vec4 v = gl_TessCoord.x * gl_in[0].gl_Position;
           + gl_TessCoord.y * gl_in[1].gl_Position;
           + gl_TessCoord.z * gl_in[2].gl_Position;
    // tePatchDistance = gl_TessCoord;
    vertex.frag_pos = (world * v).xyz;
    gl_Position = mvp * v;
    vertex.position = gl_Position.xyz;
}
