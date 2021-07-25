#version 330 core

uniform mat4 projection;
uniform mat4 view;

layout(location = 0) in vec3 v_position;

out vec3 f_position;

void main() {
    f_position = v_position;

    gl_Position = projection * view * vec4(v_position, 1.0);
}
