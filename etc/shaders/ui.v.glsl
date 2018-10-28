#version 300 es

in vec3 position;
in vec3 color;
in vec2 tex_coords;

out vec3 v_color;
out vec2 v_tex_coords;

uniform mat4 mvp;
uniform vec2 uipos;
uniform vec2 uisize;

void main()
{
    vec2 v2_pos = uipos + uisize * (position.xy - vec2(0.5, 0.5));
    vec4 v4_pos = vec4(v2_pos, 0.0, 1.0) ;
    gl_Position = mvp * v4_pos;

    v_tex_coords = tex_coords;
    v_color = color + v4_pos.xyz;
}
