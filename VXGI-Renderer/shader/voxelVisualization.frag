#version 430 core

in vec3 PositionWorld;

out vec4 color;

uniform sampler3D VoxelTexture;

uniform int VoxelDimensions;
uniform float VoxelTotalSize;

vec3 worldToVoxelIndex(vec3 worldPosition)
{
    vec3 normalizedPos = (worldPosition / (VoxelTotalSize * 0.5)) * 0.5 + 0.5;
    vec3 voxelIndex = normalizedPos * VoxelDimensions;
    
    return floor(voxelIndex);
}

bool isVoxelIndexValid(vec3 voxelIndex)
{
    return all(greaterThanEqual(voxelIndex, vec3(0.0))) && 
           all(lessThan(voxelIndex, vec3(VoxelDimensions)));
}

vec3 voxelIndexToTextureUV(vec3 voxelIndex)
{
    vec3 textureUV = (voxelIndex + 0.5) / VoxelDimensions;
    
    return textureUV;
}

vec4 sampleVoxelByIndex(vec3 voxelIndex)
{
    if (!isVoxelIndexValid(voxelIndex))
        return vec4(0.0, 0.0, 0.0, 0.0);
    
    vec3 textureUV = (voxelIndex + 0.5) / VoxelDimensions;
    return textureLod(VoxelTexture, textureUV, 0.0);
}

void main()
{
    vec3 voxelIndex = worldToVoxelIndex(PositionWorld);

    color = sampleVoxelByIndex(voxelIndex);
}