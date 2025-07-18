#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec3 PositionWorld;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
	PositionWorld = (ModelMatrix * vec4(vertexPosition, 1)).xyz;

	gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition, 1);
}