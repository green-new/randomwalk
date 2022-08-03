#version 330 core

layout (location = 0) in vec3 xyz;

out vec3 outcolor;

uniform bool dotted;

void main() {
    gl_Position = vec4(xyz, 1.0);
    if (!dotted) {
        outcolor = vec3(1.0, 1.0, 1.0);
    } else {
        outcolor = vec3(0.7686, 0.0, 0.0);
    }
}