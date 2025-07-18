#version 330 core

uniform int VoxelDimensions;
uniform float VoxelTotalSize;

void main()
{
	vec3 voxelID;
	voxelID.x = gl_VertexID % VoxelDimensions;
	voxelID.z = (gl_VertexID / VoxelDimensions) % VoxelDimensions;
	voxelID.y = (gl_VertexID / VoxelDimensions / VoxelDimensions) % VoxelDimensions;

	float voxelUnitSize = VoxelTotalSize / VoxelDimensions;

	vec3 voxelCenterPos = voxelID * voxelUnitSize - vec3(VoxelTotalSize / 2) + vec3(voxelUnitSize / 2);

	gl_Position = vec4(voxelCenterPos, 1);
}