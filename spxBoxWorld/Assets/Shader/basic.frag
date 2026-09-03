#version 460 core
out vec4 FragColor;

uniform vec3 blockColor;

void main()
{
    FragColor = vec4(blockColor, 1.0);
}