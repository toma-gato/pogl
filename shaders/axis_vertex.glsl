#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec2 screenPos;
uniform float scale;

out vec3 Color;

void main() {
    vec4 rotatedPos = view * vec4(aPos * scale, 0.0);
    
    vec4 pos = vec4(rotatedPos.xyz, 1.0);
    pos.xy = pos.xy + screenPos;
    
    gl_Position = pos;
    Color = aColor;
}