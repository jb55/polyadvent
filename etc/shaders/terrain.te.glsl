#include profile

layout(triangles, equal_spacing, cw) in;

in shader_data {
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
    vertex.shadow_coord = tc_vertices[0].shadow_coord;

    vec4 p0 = gl_TessCoord.x * gl_in[0].gl_Position;
    vec4 p1 = gl_TessCoord.y * gl_in[1].gl_Position;
    vec4 p2 = gl_TessCoord.z * gl_in[2].gl_Position;
    // tePatchDistance = gl_TessCoord;
    vec4 te_position = normalize(p0 + p1 + p2);
    gl_Position = te_position;
    vertex.position = gl_Position.xyz;
}
