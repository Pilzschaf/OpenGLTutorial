#version 120

attribute vec3 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

varying vec4 v_color;
varying vec2 v_texCoord;

void main()
{
    gl_Position = vec4(a_position, 1.0f);
    v_color = a_color;
    v_texCoord = a_texCoord;
}