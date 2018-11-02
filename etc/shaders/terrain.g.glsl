#include profile

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location = 10) in v_data {
#include shadervars.glsl
} vertices[];

out shader_data {
#include shadervars.glsl
} vertex;

#include uniforms.glsl

void main() {
    int i;

    for(i = 0;i < gl_in.length();i++)// gl_in.length() = 1 though!
    {
        vec4 v4_pos = vec4(vertices[i].position, 1.0);
        vertex.position = vertices[i].position;
        vertex.color = vertices[i].color;
        vertex.color_smooth = vertices[i].color_smooth;
        vertex.normal = vertices[i].normal;
        // vertex.shadow_coord = depth_mvp * v4_pos;
        // vertex.frag_pos = (world * v4_pos).xyz;
        vertex.shadow_coord = vertices[i].shadow_coord;
        vertex.frag_pos = vertices[i].frag_pos;

        gl_Position = gl_in[i].gl_Position;
        // gl_Position = vec4(vertices[i].position, 1.0);

        EmitVertex();
    }

    EndPrimitive();


}
