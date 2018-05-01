#version 320 es

in vec3 position;
in vec3 normal;

uniform mat4 local;
uniform mat4 mvp;

flat out float v_dot;
flat out vec3 v_norm;
flat out vec4 v_hcol;

uniform vec3 light_dir;

void main()
{
    vec4 trans_normal = vec4(normal, 1);
    gl_Position = mvp * vec4(position.xyz, 1.0);
    v_dot = dot(trans_normal, vec4(light_dir, 0));

    if (position.z <= 0.1)
      v_hcol = vec4(0.0, 0.5, 0.79, 1.0);
    else if (position.z <= 0.2)
      v_hcol = vec4(0.9176, 0.8156, 0.6588, 1.0);
    else if (position.z <= 2.0)
      v_hcol = vec4(0.6274, 0.749, 0.156, 1.0);
    else if (position.z <= 10.0)
      v_hcol = vec4(0.5, 0.5, 0.5, 1.0);
    else
      v_hcol = vec4(1.0, 1.0, 1.0, 1.0);

    v_hcol = v_hcol * 0.5;
}
