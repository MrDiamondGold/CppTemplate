#version 460 core

layout (location = 0) in vec2 f_uv;
layout (location = 1) in vec3 f_color;

out vec4 o_frag;

uniform sampler2D s_albedo;

void main()
{
    o_frag = vec4(f_color, 1.0f) * texture(s_albedo, f_uv);
}