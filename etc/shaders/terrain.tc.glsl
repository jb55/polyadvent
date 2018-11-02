#include profile

layout(vertices = 3) out;

in shader_datab {
#include shadervars.glsl
} vertex[];

out shader_dataz {
#include shadervars.glsl
} tc_vertices[];

#include uniforms.glsl

// uniform float TessLevelInner;
// uniform float TessLevelOuter;


void main()
{
    tc_vertices[gl_InvocationID].position = vertex[gl_InvocationID].position;
    tc_vertices[gl_InvocationID].color = vertex[gl_InvocationID].color;
    tc_vertices[gl_InvocationID].color_smooth = vertex[gl_InvocationID].color_smooth;
    tc_vertices[gl_InvocationID].normal = vertex[gl_InvocationID].normal;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    const float inner = 5.0;
    const float outer = 8.0;
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = inner;
        gl_TessLevelOuter[0] = outer;
        gl_TessLevelOuter[1] = outer;
        gl_TessLevelOuter[2] = outer;
    }
}
