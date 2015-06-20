#version 110

attribute vec2 position;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position * 0.5, 0.0, 1.0);
}