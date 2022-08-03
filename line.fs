#version 330 core

in vec3 outcolor;
out vec4 FragColor;

void main() {
    FragColor = vec4(outcolor, 1.0);
}