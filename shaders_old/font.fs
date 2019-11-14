#version 120

varying vec2 v_tex_coord;

uniform sampler2D u_texture;

void main()
{
    gl_FragColor = vec4(texture2D(u_texture, v_tex_coord).a);
}