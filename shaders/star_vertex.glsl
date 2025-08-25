#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aBrightness;
layout (location = 2) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 starColor;
out float brightness;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aBrightness * 3.0;
    starColor = aColor;
    brightness = aBrightness;
}