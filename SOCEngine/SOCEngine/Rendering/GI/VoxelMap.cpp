#include "VoxelMap.h"
#include "Director.h"

using namespace Device;
using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::View;

VoxelMap::VoxelMap()
	: Texture3D()
{
}

VoxelMap::~VoxelMap()
{
	VoxelMap::Destroy();
}

void VoxelMap::Initialize(
	uint sideLength, DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint mipmapCount, bool isAnisotropic)
{
	_sideLength		= sideLength;
	_mipmapCount	= mipmapCount;

	uint width		= sideLength * (isAnisotropic ? (uint)Direction::Num : 1);
	uint height		= sideLength;
	uint depth		= sideLength;
	Texture3D::Initialize(width, height, depth, typelessFormat, srvFormat, uavFormat, D3D11_BIND_RENDER_TARGET, _mipmapCount);

	for(uint i=1; i<mipmapCount; ++i)
	{
		UnorderedAccessView* uav = new UnorderedAccessView;
		uav->Initialize(uavFormat, 0, _texture, D3D11_UAV_DIMENSION_TEXTURE3D, i, -1);

		_mipmapUAVs.push_back(uav);
	}
}

void VoxelMap::Destroy()
{
	for(auto iter = _mipmapUAVs.begin(); iter != _mipmapUAVs.end(); ++iter)
	{
		(*iter)->Destroy();
		SAFE_DELETE(*iter);
	}
	
	_mipmapUAVs.clear();
	Texture3D::Destroy();
}