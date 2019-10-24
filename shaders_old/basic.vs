#version 120

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec2 a_tex_coord;

varying vec3 v_position;
varying vec2 v_tex_coord;
varying mat3 v_tbn;

uniform mat4 u_modelViewProj;
uniform mat4 u_modelView;
uniform mat4 u_invModelView;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0f);

    vec3 t = normalize(mat3(u_invModelView) * a_tangent);
    vec3 n = normalize(mat3(u_invModelView) * a_normal);
    t = normalize(t - dot(t, n) * n); // Reorthogonalize with Gram-Schmidt process
    vec3 b = normalize(mat3(u_invModelView) * cross(n, t));
    mat3 tbn = transpose(mat3(t, b, n)); // transpose is equal to inverse in this case
    v_tbn = tbn;

    v_position = vec3(u_modelView * vec4(a_position, 1.0f));
    v_tex_coord = a_tex_coord;
}