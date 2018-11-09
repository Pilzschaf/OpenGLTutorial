#version 120

attribute vec3 a_position;
attribute vec4 a_color;

varying vec4 v_color;

void main()
{
    gl_Position = vec4(a_position, 1.0f);
    v_color = a_color;
}