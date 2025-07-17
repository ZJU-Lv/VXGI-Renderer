#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 ModelMatrix;
uniform mat4 LightModelViewProjectionMatrix;

out VertexData
{
    vec3 normal;
    vec2 texCoord;
    vec4 positionLightSpace;
} vertOut;

void main()
{
    vertOut.normal = normalize(ModelMatrix * vec4(vertexNormal, 0)).xyz;
    vertOut.texCoord = vertexTexCoord;
    vertOut.positionLightSpace = LightModelViewProjectionMatrix * vec4(vertexPosition, 1);
	vertOut.positionLightSpace.xyz = vertOut.positionLightSpace.xyz * 0.5f + 0.5f;

    gl_Position = ModelMatrix * vec4(vertexPosition, 1);
}