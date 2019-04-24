#version 120

varying vec3 v_normal;
varying vec3 v_position;

uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform vec3 u_emissive;
uniform float u_shininess;

void main()
{
    vec3 view = normalize(-v_position);
    vec3 light = normalize(vec3(1.0f, 1.0f, 1.0f));
    vec3 normal = normalize(v_normal);
    vec3 reflection = reflect(-light, normal);

    vec3 ambient = u_diffuse * 0.2;
    vec3 diffuse = max(dot(normal, light), 0.0) * u_diffuse;
    vec3 specular = pow(max(dot(reflection, view), 0.000001), u_shininess) * u_specular;

    gl_FragColor = vec4(ambient + diffuse + specular + u_emissive, 1.0f);
}