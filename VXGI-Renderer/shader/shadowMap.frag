#version 330 core

in float lightSpaceDepth;

out vec4 color;

void main()
{
    color = vec4(vec3(lightSpaceDepth), 1.0);
}