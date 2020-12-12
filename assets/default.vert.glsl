#version 460 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_color;
layout (location = 3) in vec2 v_uv;

out vec2 f_uv;
out vec3 f_color;

void main()
{
    f_uv = v_uv;
    f_color = v_color;
    gl_Position = vec4(v_position, 1.0);
}