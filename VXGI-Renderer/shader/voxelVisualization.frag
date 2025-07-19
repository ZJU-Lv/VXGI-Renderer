#version 430 core

uniform sampler3D VoxelTexture;

uniform int VoxelDimensions;
uniform float VoxelTotalSize;

in vec3 PositionWorld;

out vec4 color;

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

vec3 linearToSRGB(vec3 linear)
{
    return pow(linear, vec3(1.0 / 2.2));
}

void main()
{
    color = sampleFromVoxelTexture(PositionWorld);

    color.rgb = linearToSRGB(color.rgb);
}