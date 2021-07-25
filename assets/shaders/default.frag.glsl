#version 460 core

layout (location = 0) in vec2 f_uv;

out vec4 o_frag;

uniform sampler2D s_albedo;

void main()
{
    vec4 color = texture(s_albedo, f_uv);
    if(color.a < 0.5) {
        discard;
    }
    //color.rgb = pow(color.rgb, vec3(1.0 / 2.2));

    o_frag = color;
}