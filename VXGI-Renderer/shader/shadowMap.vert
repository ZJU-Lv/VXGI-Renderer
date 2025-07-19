#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 LightModelViewProjectionMatrix;

out float lightSpaceDepth;

void main()
{
	gl_Position = LightModelViewProjectionMatrix * vec4(vertexPosition, 1);
	lightSpaceDepth = gl_Position.z * 0.5 + 0.5;
}