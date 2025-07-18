#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

uniform sampler3D VoxelTexture;

uniform int VoxelDimensions;
uniform float VoxelTotalSize;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 PositionWorld;

vec4 sampleFromVoxelTexture(vec3 worldPosition)
{
    vec3 voxelTextureUV = worldPosition / (VoxelTotalSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5;
    if(any(lessThan(voxelTextureUV, vec3(0.0))) || any(greaterThan(voxelTextureUV, vec3(1.0))))
        return vec4(0.0, 0.0, 0.0, 0.0);    // out of range
    
    float BorderOffset = 0.5 / VoxelDimensions;
    voxelTextureUV = clamp(voxelTextureUV, vec3(BorderOffset), vec3(1.0 - BorderOffset));
    
    return textureLod(VoxelTexture, voxelTextureUV, 0.0);
}

void main()
{
    vec4 sampleColor = sampleFromVoxelTexture(gl_in[0].gl_Position.xyz);
    if(sampleColor.a < 0.1)
        return;

    float voxelUnitSize = VoxelTotalSize / VoxelDimensions;

    vec3 v1World = gl_in[0].gl_Position.xyz + vec3(-0.5, 0.5, 0.5) * voxelUnitSize;
    vec3 v2World = gl_in[0].gl_Position.xyz + vec3(0.5, 0.5, 0.5) * voxelUnitSize;
    vec3 v3World = gl_in[0].gl_Position.xyz + vec3(-0.5, -0.5, 0.5) * voxelUnitSize;
    vec3 v4World = gl_in[0].gl_Position.xyz + vec3(0.5, -0.5, 0.5) * voxelUnitSize;
    vec3 v5World = gl_in[0].gl_Position.xyz + vec3(-0.5, 0.5, -0.5) * voxelUnitSize;
    vec3 v6World = gl_in[0].gl_Position.xyz + vec3(0.5, 0.5, -0.5) * voxelUnitSize;
    vec3 v7World = gl_in[0].gl_Position.xyz + vec3(-0.5, -0.5, -0.5) * voxelUnitSize;
    vec3 v8World = gl_in[0].gl_Position.xyz + vec3(0.5, -0.5, -0.5) * voxelUnitSize;

    vec4 v1Clip = ProjectionMatrix * ViewMatrix * vec4(v1World, 1);
	vec4 v2Clip = ProjectionMatrix * ViewMatrix * vec4(v2World, 1);
	vec4 v3Clip = ProjectionMatrix * ViewMatrix * vec4(v3World, 1);
	vec4 v4Clip = ProjectionMatrix * ViewMatrix * vec4(v4World, 1);
	vec4 v5Clip = ProjectionMatrix * ViewMatrix * vec4(v5World, 1);
	vec4 v6Clip = ProjectionMatrix * ViewMatrix * vec4(v6World, 1);
	vec4 v7Clip = ProjectionMatrix * ViewMatrix * vec4(v7World, 1);
	vec4 v8Clip = ProjectionMatrix * ViewMatrix * vec4(v8World, 1);

    PositionWorld = v1World;
    gl_Position = v1Clip;
    EmitVertex();
    PositionWorld = v3World;
    gl_Position = v3Clip;
    EmitVertex();
    PositionWorld = v4World;
    gl_Position = v4Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v1World;
    gl_Position = v1Clip;
    EmitVertex();
    PositionWorld = v4World;
    gl_Position = v4Clip;
    EmitVertex();
    PositionWorld = v2World;
    gl_Position = v2Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v6World;
    gl_Position = v6Clip;
    EmitVertex();
    PositionWorld = v8World;
    gl_Position = v8Clip;
    EmitVertex();
    PositionWorld = v7World;
    gl_Position = v7Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v6World;
    gl_Position = v6Clip;
    EmitVertex();
    PositionWorld = v7World;
    gl_Position = v7Clip;
    EmitVertex();
    PositionWorld = v5World;
    gl_Position = v5Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v2World;
    gl_Position = v2Clip;
    EmitVertex();
    PositionWorld = v4World;
    gl_Position = v4Clip;
    EmitVertex();
    PositionWorld = v8World;
    gl_Position = v8Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v2World;
    gl_Position = v2Clip;
    EmitVertex();
    PositionWorld = v8World;
    gl_Position = v8Clip;
    EmitVertex();
    PositionWorld = v6World;
    gl_Position = v6Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v5World;
    gl_Position = v5Clip;
    EmitVertex();
    PositionWorld = v7World;
    gl_Position = v7Clip;
    EmitVertex();
    PositionWorld = v3World;
    gl_Position = v3Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v5World;
    gl_Position = v5Clip;
    EmitVertex();
    PositionWorld = v3World;
    gl_Position = v3Clip;
    EmitVertex();
    PositionWorld = v1World;
    gl_Position = v1Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v5World;
    gl_Position = v5Clip;
    EmitVertex();
    PositionWorld = v1World;
    gl_Position = v1Clip;
    EmitVertex();
    PositionWorld = v2World;
    gl_Position = v2Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v5World;
    gl_Position = v5Clip;
    EmitVertex();
    PositionWorld = v2World;
    gl_Position = v2Clip;
    EmitVertex();
    PositionWorld = v6World;
    gl_Position = v6Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v3World;
    gl_Position = v3Clip;
    EmitVertex();
    PositionWorld = v7World;
    gl_Position = v7Clip;
    EmitVertex();
    PositionWorld = v8World;
    gl_Position = v8Clip;
    EmitVertex();
    EndPrimitive();

    PositionWorld = v3World;
    gl_Position = v3Clip;
    EmitVertex();
    PositionWorld = v8World;
    gl_Position = v8Clip;
    EmitVertex();
    PositionWorld = v4World;
    gl_Position = v4Clip;
    EmitVertex();
    EndPrimitive();
}