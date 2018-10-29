#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

out vec4 v_color;

void main()
{
    gl_Position = vec4(a_position, 1.0f);
    v_color = a_color;
}