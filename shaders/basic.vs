#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec4 v_color;

uniform mat4 u_modelViewProj;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0f);
    v_color = vec4(a_normal, 1.0f);
}
