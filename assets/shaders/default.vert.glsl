#version 460 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec4 v_tangent;
layout (location = 3) in vec2 v_uv;

out vec2 f_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    f_uv = v_uv;

    gl_Position = u_projection * u_view * u_model * vec4(v_position, 1.0);
}