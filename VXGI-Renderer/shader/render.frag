#version 430 core

in vec3 PositionWorld;
in vec4 PositionLightSpace;
in vec2 texCoord;
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

const int ConeNum = 6;
vec3 ConeDirections[6] = vec3[]
(vec3(0, 1, 0),
 vec3(0, 0.5, 0.866025),
 vec3(0.823639, 0.5, 0.267617),
 vec3(0.509037, 0.5, -0.700629),
 vec3(-0.509037, 0.5, -0.700629),
 vec3(-0.823639, 0.5, 0.267617));
float ConeWeights[6] = float[](0.25, 0.15, 0.15, 0.15, 0.15, 0.15);

bool isPositionInVoxelTexture(vec3 worldPosition)
{
    vec3 voxelTextureUV = worldPosition / (VoxelTotalSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5;
    if(any(lessThan(voxelTextureUV, vec3(0.0))) || any(greaterThan(voxelTextureUV, vec3(1.0))))
        return false;
    else
        return true;
}

vec4 sampleFromVoxelTexture(vec3 worldPosition, float lod)
{
    vec3 voxelTextureUV = worldPosition / (VoxelTotalSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5;
    if(any(lessThan(voxelTextureUV, vec3(0.0))) || any(greaterThan(voxelTextureUV, vec3(1.0))))
        return vec4(0.0, 0.0, 0.0, 0.0);    // out of range
    
    const float BorderOffset = 0.5 / VoxelDimensions;
    voxelTextureUV = clamp(voxelTextureUV, vec3(BorderOffset), vec3(1.0 - BorderOffset));
    
    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

vec4 coneTrace(vec3 direction, float tanHalfAngle)
{
    vec3 colorAccumulate = vec3(0);
    float occlusionAccumulate = 0.0;

    float voxelUnitSize = VoxelTotalSize / VoxelDimensions;
    vec3 startPos = PositionWorld + direction * voxelUnitSize * 0.5; // start half a voxel away to avoid self occlusion

    //vec4 startPosColor = sampleFromVoxelTexture(startPos, 0.0);
    //if(startPosColor.a > 0.95)    // cone is occluded
    //    return vec4(colorAccumulate, startPosColor.a);

    float distance = voxelUnitSize;
    float alpha = 0.0;
    while(distance < MaxDistance && alpha < 0.95)
    {
        vec3 currentPos = startPos + distance * direction;
        if(!isPositionInVoxelTexture(currentPos))
            break;

        float diameter = max(voxelUnitSize, 2.0 * tanHalfAngle * distance);
        float lodLevel = log2(diameter / voxelUnitSize);
        vec4 voxelColor = sampleFromVoxelTexture(currentPos, lodLevel);

        float voxelAlpha = voxelColor.a;
        float weight = (1.0 - alpha) * voxelAlpha;

        colorAccumulate += weight * voxelColor.rgb;
        alpha += weight;

        if(ShowAmbientOcculision > 0)
        {
            float occlusionContribution = voxelAlpha / (1.0 + 0.03 * distance);
            occlusionAccumulate += (1 - occlusionAccumulate) * occlusionContribution;
        }

        distance += diameter * 0.5;
    }

    return vec4(colorAccumulate, occlusionAccumulate);
}

vec4 indirectDiffuse(mat3 tangentSpaceToWorldSpace)
{
    vec3 totalColor = vec3(0.0);
    float totalOcclusion = 0.0;
    for(int i = 0; i < ConeNum; i++)
    {
        vec3 coneDir = tangentSpaceToWorldSpace * ConeDirections[i];
        vec4 coneTraceResult = coneTrace(coneDir, 0.577);  // half of cone angle is 30, tan(30) = 0.577

        totalColor += ConeWeights[i] * coneTraceResult.rgb;
        totalOcclusion += ConeWeights[i] * coneTraceResult.a;
    }

    float aoFactor = 1.0 - clamp(totalOcclusion, 0.0, 1.0);
    aoFactor *= aoFactor;

    return vec4(totalColor, aoFactor);
}

vec4 indirectSpecular(vec3 coneDir)
{
    return coneTrace(coneDir, 0.07);  // tan(4) = 0.07
}

vec3 calculateNormalFromHeightMap()
{
    vec2 offset = vec2(1.0) / HeightTextureSize;
    float fragmentHeight = texture(HeightTexture, texCoord).r;
    float diffX = texture(HeightTexture, texCoord + vec2(offset.x, 0.0)).r - fragmentHeight;
    float diffY = texture(HeightTexture, texCoord + vec2(0.0, offset.y)).r - fragmentHeight;

    float factor = -3.0;
    vec3 calculateNormal = normalize(vec3(factor*diffX, 1.0, factor*diffY));

    return calculateNormal;
}

vec3 linearToSRGB(vec3 linear)
{
    return pow(linear, vec3(1.0 / 2.2));
}

void main()
{
    mat3 tangentSpaceToWorldSpace = inverse(transpose(mat3(TangentWorld, NormalWorld, BitangentWorld)));

    vec3 heightMapNormal = normalize(tangentSpaceToWorldSpace * calculateNormalFromHeightMap());
    vec3 toLight = normalize(ToLightDirection);
    vec3 toCamera = normalize(CameraPosition - PositionWorld);

    vec4 materialColor = texture(DiffuseTexture, texCoord);

    float visibility = texture(ShadowMap, vec3(PositionLightSpace.xy, (PositionLightSpace.z - 0.0005) / PositionLightSpace.w));

    // direct diffuse light
    float cosTheta = max(0, dot(heightMapNormal, toLight));
    vec3 directDiffuseLight = ShowDirect > 0 ? visibility * cosTheta * materialColor.rgb : vec3(0.0);

    // indirect diffuse light
    vec4 diffuseResult = 4.0 * indirectDiffuse(tangentSpaceToWorldSpace);
    float aoFactor = diffuseResult.a;
    vec3 indirectDiffuseLight = ShowIndirectDiffuse > 0 ? diffuseResult.rgb * materialColor.rgb : vec3(0.0);

    // indirect specular light
    vec4 specularColor = texture(SpecularTexture, texCoord);
    specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;
    vec3 reflectDir = reflect(-toCamera, heightMapNormal);
    vec3 indirectSpecularLight = ShowIndirectSpecular > 0 ? indirectSpecular(reflectDir).rgb * materialColor.rgb : vec3(0.0);

    vec3 compositeColor = directDiffuseLight + (indirectDiffuseLight + indirectSpecularLight) * aoFactor;

    if(ShowDirect < 1 && ShowIndirectDiffuse < 1 && ShowIndirectSpecular < 1)
        compositeColor = vec3(1.0) * aoFactor;

    // gamma correction
    compositeColor = linearToSRGB(compositeColor);

    color = vec4(compositeColor, Opacity);
}