#version 450 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 cometColor;

out vec4 FragColor;

void main() {
    vec3 viewPos = vec3(0.0, 0.0, 5.0);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    float intensity = pow(dot(viewDir, norm), 2.0);
    vec3 color = mix(cometColor, vec3(1.0), intensity);

    FragColor = vec4(color, 0.6);
}