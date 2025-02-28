#version 330 core

uniform vec3 sphereColor;

out vec4 FragColor;


void main() {

    FragColor = vec4(sphereColor, 1.0);
}