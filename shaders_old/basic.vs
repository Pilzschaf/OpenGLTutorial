#version 120

attribute vec3 a_position;
attribute vec4 a_color;

varying vec4 v_color;

uniform mat4 u_modelViewProj;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0f);
    v_color = a_color;
}