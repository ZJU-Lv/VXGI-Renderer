#version 330 core

layout(location = 0) in vec3 vertexPosition_model;

uniform mat4 LightModelViewProjectionMatrix;

void main()
{
	gl_Position = LightModelViewProjectionMatrix * vec4(vertexPosition_model, 1);
}