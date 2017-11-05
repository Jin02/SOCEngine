#include "VoxelMap.h"

using namespace Device;
using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::View;

void VoxelMap::Initialize(
	DirectX& dx,
	uint sideLength, DXGI_FORMAT tex3DFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint mipmapCount, bool isAnisotropic)
{
	_sideLength		= sideLength;
	_mipmapCount	= mipmapCount;

	uint width		= sideLength * (isAnisotropic ? (uint)Direction::Num : 1);
	uint height		= sideLength;
	uint depth		= sideLength;

	_tex3D.Initialize(dx, width, height, depth, tex3DFormat, srvFormat, uavFormat, D3D11_BIND_RENDER_TARGET, _mipmapCount);

	for(uint i=1; i<mipmapCount; ++i)
	{
		UnorderedAccessView uav;
		uav.Initialize(dx, uavFormat, 0, _tex3D.GetTexture(), D3D11_UAV_DIMENSION_TEXTURE3D, i, -1);

		_mipmapUAVs.push_back(uav);
	}
}