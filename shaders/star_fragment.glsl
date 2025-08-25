#version 450 core

in vec3 starColor;
in float brightness;
out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    if (dist > 0.5) discard;
    
    float alpha = (1.0 - dist * 2.0) * brightness;
    FragColor = vec4(starColor, alpha);
}