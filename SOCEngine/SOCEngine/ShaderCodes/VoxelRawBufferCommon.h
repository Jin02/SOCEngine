#ifndef __SOC_VOXEL_RAW_BUFFER_COMMON_H__
#define __SOC_VOXEL_RAW_BUFFER_COMMON_H__

uint GetFlattedVoxelIndex(uint3 voxelIndex, uint cascade, uint dimension)
{
	uint sqDimension	= dimension * dimension;
	uint offset			= (sqDimension * dimension) * cascade;
	uint flat			= voxelIndex.x + (voxelIndex.y * dimension) + (voxelIndex.z * sqDimension);
	return offset + flat;
}

uint GetFlattedVoxelIndexWithFaceIndex(uint3 voxelIndex, uint cascade, uint faceIndex, uint dimension)
{
	uint sqDimension	= dimension * dimension;

	uint fullLength		= (sqDimension * dimension);
	uint faceOffset		= fullLength * faceIndex;
	uint cascadeOffset	= fullLength * 6 * cascade;

	uint localFlattedIdx	= voxelIndex.x + (voxelIndex.y * dimension) + (voxelIndex.z * sqDimension);
	return cascadeOffset + faceOffset  +  localFlattedIdx;
}

#endif
