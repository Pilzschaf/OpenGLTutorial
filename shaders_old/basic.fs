#version 120

varying vec4 v_color;

uniform vec4 u_color;

void main()
{
    gl_FragColor = u_color;
}