#version 120

varying vec4 v_color;
varying vec2 v_texCoord;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    gl_FragColor = texColor * u_color;
}