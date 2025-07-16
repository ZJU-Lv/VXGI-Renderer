#version 430
#extension GL_ARB_shader_image_load_store : enable

layout(rgba8) writeonly uniform image3D VoxelTexture;

uniform sampler2D DiffuseTexture;
uniform sampler2DShadow ShadowMap;

uniform vec3 ToLightDirection;
uniform int VoxelDimensions;

in GeomData
{
	vec3 normal;
    vec2 texCood;
    flat int axis;            // 1 represent X, 2 represent Y, 3 represent Z
    vec4 positionLightSpace;
} geomOut;

void main()
{
    ivec3 fragmentPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, VoxelDimensions * gl_FragCoord.z);
	ivec3 voxelPos;
	if(geomOut.axis == 1)
	{
	    voxelPos.x = VoxelDimensions - fragmentPos.z - 1;
		voxelPos.y = fragmentPos.y;
		voxelPos.z = VoxelDimensions - fragmentPos.x - 1;
	}
	else if(geomOut.axis == 2)
	{
		voxelPos.x = fragmentPos.x;
		voxelPos.y = VoxelDimensions - fragmentPos.z - 1;
	    voxelPos.z = VoxelDimensions - fragmentPos.y - 1;
	}
	else
	{
		voxelPos.x = fragmentPos.x;
		voxelPos.y = fragmentPos.y;
	    voxelPos.z = VoxelDimensions - fragmentPos.y - 1;
	}

	vec4 materialColor = texture(DiffuseTexture, geomOut.texCood);

	vec3 lightDir = normalize(ToLightDirection);
	float cosTheta = max(0, dot(geomOut.normal, lightDir));

	float visibility = texture(ShadowMap, vec3(geomOut.positionLightSpace.xy, (geomOut.positionLightSpace.z - 0.005) / geomOut.positionLightSpace.w));

    imageStore(VoxelTexture, voxelPos, vec4(materialColor.rgb * cosTheta * visibility, materialColor.a));
}