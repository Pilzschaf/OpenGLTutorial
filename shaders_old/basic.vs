#version 120

attribute vec3 a_position;
attribute vec3 a_normal;

varying vec3 v_normal;
varying vec3 v_position;

uniform mat4 u_modelViewProj;
uniform mat4 u_modelView;
uniform mat4 u_invModelView;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0f);
    v_normal = mat3(u_invModelView) * a_normal;
    v_position = vec3(u_modelView * vec4(a_position, 1.0f));
}