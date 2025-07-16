#version 430 core

in vec3 PositionWorld;
in vec4 PositionLightSpace;
in vec2 texCood;
in vec3 NormalWorld;
in vec3 TangentWorld;
in vec3 BitangentWorld;

out vec4 color;

uniform vec3 CameraPosition;
uniform vec3 ToLightDirection;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D HeightTexture;
uniform vec2 HeightTextureSize;

uniform float Shininess;
uniform float Opacity;

uniform sampler2DShadow ShadowMap;
uniform sampler3D VoxelTexture;

uniform int VoxelDimensions;
uniform float VoxelTotalSize;

uniform int ShowDirect;
uniform int ShowIndirectDiffuse;
uniform int ShowIndirectSpecular;
uniform int ShowAmbientOcculision;

const float MaxDistance = 100.0;
const float OcclusionThresh = 0.95;

const int ConeNum = 6;
vec3 ConeDirections[6] = vec3[]
(vec3(0, 1, 0),
 vec3(0, 0.5, 0.866025),
 vec3(0.823639, 0.5, 0.267617),
 vec3(0.509037, 0.5, -0.700629),
 vec3(-0.509037, 0.5, -0.700629),
 vec3(-0.823639, 0.5, 0.267617));
float ConeWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);

vec4 sampleFromVoxelTexture(vec3 worldPosition, float lod)
{
    vec3 voxelTextureUV = worldPosition / (VoxelTotalSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5;
    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

vec3 coneTrace(vec3 direction, float tanHalfAngle)
{
    vec3 color = vec3(0);
    float occlusionAccumulate = 0.0;

    float voxelUnitSize = VoxelTotalSize / VoxelDimensions;
    vec3 startPos = PositionWorld + NormalWorld * voxelUnitSize * 0.5; // start half a voxel away to avoid self occlusion

    float distance = voxelUnitSize;
    while(distance < MaxDistance && occlusionAccumulate < OcclusionThresh)
    {
        float diameter = max(voxelUnitSize, 2.0 * tanHalfAngle * distance);
        float lodLevel = log2(diameter / voxelUnitSize);
        vec4 voxelColor = sampleFromVoxelTexture(startPos + distance * direction, lodLevel);

        float leakRatio = 1.0 - occlusionAccumulate;
        color += leakRatio * voxelColor.rgb;
        occlusionAccumulate += leakRatio * voxelColor.a;
        distance += diameter * 0.5;
    }

    return color;
}

vec3 indirectDiffuse(mat3 tangentSpaceToWorldSpace)
{
    vec3 diffuseColor = vec3(0);
    for(int i = 0; i < ConeNum; i++)
        diffuseColor += ConeWeights[i] * coneTrace(tangentSpaceToWorldSpace * ConeDirections[i], 0.577);  // half of cone angle is 30, tan(30) = 0.577

    return diffuseColor;
}

vec3 calculateNormalFromHeightMap()
{
    vec2 offset = vec2(1.0) / HeightTextureSize;
    float fragmentHeight = texture(HeightTexture, texCood).r;
    float diffX = texture(HeightTexture, texCood + vec2(offset.x, 0.0)).r - fragmentHeight;
    float diffY = texture(HeightTexture, texCood + vec2(0.0, offset.y)).r - fragmentHeight;

    float factor = -3.0;
    vec3 calculateNormal = normalize(vec3(factor*diffX, 1.0, factor*diffY));

    return calculateNormal;
}

vec3 linearToSRGB(vec3 linear)
{
    return pow(linear, vec3(1.0/2.2));
}

void main()
{
    mat3 tangentSpaceToWorldSpace = inverse(transpose(mat3(TangentWorld, NormalWorld, BitangentWorld)));

    vec3 heightMapNormal = normalize(tangentSpaceToWorldSpace * calculateNormalFromHeightMap());
    vec3 toLight = normalize(ToLightDirection);
    vec3 toCamera = normalize(CameraPosition - PositionWorld);

    vec4 materialColor = texture(DiffuseTexture, texCood);

    float visibility = texture(ShadowMap, vec3(PositionLightSpace.xy, (PositionLightSpace.z - 0.0005) / PositionLightSpace.w));

    // direct diffuse light
    float cosTheta = max(0, dot(heightMapNormal, toLight));
    vec3 directDiffuseLight = ShowDirect > 0 ? visibility * cosTheta * materialColor.rgb : vec3(0.0);

    // indirect diffuse light
    vec3 indirectDiffuseLight = vec3(0);
    if(ShowIndirectDiffuse > 0)
    indirectDiffuseLight = indirectDiffuse(tangentSpaceToWorldSpace) * materialColor.rgb;

    // indirect specular light
    vec4 specularColor = texture(SpecularTexture, texCood);
    specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;
    vec3 reflectDir = normalize(-toCamera - 2.0 * dot(-toCamera, heightMapNormal) * heightMapNormal);
    vec3 indirectSpecularLight = vec3(0);
    if(ShowIndirectSpecular > 0)
    indirectSpecularLight = coneTrace(reflectDir, 0.07) * materialColor.rgb;    // atan(0.07) = 4 degrees

    color = vec4(directDiffuseLight + indirectDiffuseLight + indirectSpecularLight, materialColor.a);

    // gamma correction
    color.rgb = linearToSRGB(color.rgb);
}