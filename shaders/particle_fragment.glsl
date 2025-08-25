#version 450 core

in vec3 particleColor;
out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    float alpha = 1.0 - smoothstep(0.0, 0.5, dist);
    vec3 glowColor = mix(particleColor, vec3(1.0), 0.2);

    FragColor = vec4(glowColor, alpha * 0.7);
}
