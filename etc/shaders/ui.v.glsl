#version 300 es

in vec3 position;
in vec3 color;

out vec3 v_color;

uniform mat4 mvp;
uniform vec2 uipos;
uniform vec2 uisize;

uniform sampler2D texture1;

void main()
{
    vec2 v2_pos = uipos + uisize * (position.xy - vec2(0.5, 0.5)) - (1.0 - uisize);
    vec4 v4_pos = vec4(v2_pos, 0.0, 1.0) ;
    gl_Position = mvp * v4_pos;


    v_color = color + position;
}
