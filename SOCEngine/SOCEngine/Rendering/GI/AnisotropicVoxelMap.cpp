#include "AnisotropicVoxelMap.h"

using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;

AnisotropicVoxelMap::AnisotropicVoxelMap()
{
	memset(_voxelMaps, 0, sizeof(_voxelMaps));
}

AnisotropicVoxelMap::~AnisotropicVoxelMap()
{
	Destroy();

	for(uint i=0; i<(uint)Direction::Num; ++i)
		SAFE_DELETE(_voxelMaps[i]);
}

void AnisotropicVoxelMap::Initialize(uint sideLength, DXGI_FORMAT format, uint mipmapLevels)
{
	_sideLength = sideLength;
	_mipmapLevels = mipmapLevels;

	for(uint i=0; i<(uint)Direction::Num; ++i)
	{
		VoxelMap* voxelMap = new VoxelMap;
		voxelMap->Initialize(Size<uint>(sideLength, sideLength), sideLength, format);

		_voxelMaps[i] = voxelMap;
	}
}

void AnisotropicVoxelMap::Destroy()
{
	for(uint i=0; i<(uint)Direction::Num; ++i)
		_voxelMaps[i]->Destroy();
}

void AnisotropicVoxelMap::BindUAVsToContext(const Device::DirectX* dx, const uint bindSlotOffset)
{
	for(uint i=0; i<(uint)Direction::Num; ++i)
	{
	}
}