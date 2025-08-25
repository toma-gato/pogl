#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;

uniform mat4 view;
uniform mat4 projection;

out vec3 particleColor;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize;
    particleColor = aColor;
}