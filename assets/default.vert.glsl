#version 460 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec4 v_tangent;
layout (location = 3) in vec2 v_uv;

out vec2 f_uv;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    f_uv = v_uv;

    gl_Position = projection * view * vec4(v_position, 1.0);
}