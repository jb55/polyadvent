#version 300 es

in vec3 position;
in vec3 color;

out vec3 v_color;

uniform mat4 view;

uniform sampler2D texture1;

void main()
{
    vec4 v4_pos = vec4(position * 0.2, 1.0);
    gl_Position = v4_pos;

    v_color = color + position;
}
