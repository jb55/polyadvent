#version 110

attribute vec3 position;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position.xyz * 0.5, 1.0);
}