#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec3 PositionWorld;
out vec4 PositionLightSpace;
out vec2 texCoord;
out vec3 NormalWorld;
out vec3 TangentWorld;
out vec3 BitangentWorld;

uniform vec3 CameraPosition;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 LightModelViewProjectionMatrix;

void main()
{
	PositionWorld = (ModelMatrix * vec4(vertexPosition, 1)).xyz;

	PositionLightSpace = LightModelViewProjectionMatrix * vec4(vertexPosition, 1);
	PositionLightSpace.xyz = PositionLightSpace.xyz * 0.5 + 0.5;

	texCoord = vertexTexCoord;
	NormalWorld = normalize((ModelMatrix * vec4(vertexNormal, 0)).xyz);
	TangentWorld = normalize((ModelMatrix * vec4(vertexTangent, 0)).xyz);
	BitangentWorld = normalize((ModelMatrix * vec4(vertexBitangent, 0)).xyz);

	gl_Position =  ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition, 1);
}