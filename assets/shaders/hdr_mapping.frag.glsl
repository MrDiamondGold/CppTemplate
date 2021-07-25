#version 330 core

uniform sampler2D s_hdr;

in vec3 f_position;

out vec4 o_color;

const vec3 inv_atan = vec2(0.1591, 0.3183);

vec2 sampler_spherical_map(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    
    uv *= inv_atan;
    uv += 0.5;

    return uv;
}

void main() {
    vec2 uv = sampler_spherical_map(normalize(f_position));
    vec3 color = texture(s_hdr, uv).rgb;

    o_color = vec4(color, 1.0);
}