#include profile

layout(vertices = 3) out;

in shader_data {
#include shadervars.glsl
} data_in[];

out shader_data {
#include shadervars.glsl
} data_out[];

// #include uniforms.glsl

// uniform float TessLevelInner;
// uniform float TessLevelOuter;


void main()
{

    // gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    const float inner = 1.0;
    const float outer = 2.0;

    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = inner;
        gl_TessLevelOuter[0] = outer;
        gl_TessLevelOuter[1] = outer;
        gl_TessLevelOuter[2] = outer;
    }

    // data_out[gl_InvocationID] = data_in[gl_InvocationID];
    data_out[gl_InvocationID].position     = data_in[gl_InvocationID].position;
    data_out[gl_InvocationID].color        = data_in[gl_InvocationID].color;
    data_out[gl_InvocationID].color_smooth = data_in[gl_InvocationID].color_smooth;
    data_out[gl_InvocationID].normal       = data_in[gl_InvocationID].normal;
}
