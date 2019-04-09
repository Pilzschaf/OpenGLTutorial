#version 120

varying vec3 v_normal;
varying vec3 v_position;

void main()
{
    vec3 view = normalize(-v_position);
    vec3 light = normalize(vec3(1.0f, 1.0f, 1.0f));
    vec3 normal = normalize(v_normal);
    vec3 color = vec3(0.4f, 0.2f, 0.1f);
    vec3 reflection = reflect(-light, normal);

    vec3 ambient = color * 0.2;
    vec3 diffuse = max(dot(normal, light), 0.0) * color;
    vec3 specular = pow(max(dot(reflection, view), 0.0), 4.0) * color;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0f);
}