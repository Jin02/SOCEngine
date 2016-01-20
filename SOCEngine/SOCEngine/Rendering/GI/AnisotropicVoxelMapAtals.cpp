#include "AnisotropicVoxelMapAtlas.h"
#include "Director.h"

using namespace Device;
using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::View;

AnisotropicVoxelMapAtlas::AnisotropicVoxelMapAtlas()
	: Texture3D()
{
}

AnisotropicVoxelMapAtlas::~AnisotropicVoxelMapAtlas()
{
	AnisotropicVoxelMapAtlas::Destory();
}

void AnisotropicVoxelMapAtlas::Initialize(
	uint sideLength, uint maxNumOfCascade, DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint mipmapCount)
{
	_sideLength		= sideLength;
	_mipmapCount	= mipmapCount;

	uint width		= sideLength * (uint)Direction::Num;
	uint height		= sideLength * maxNumOfCascade;
	uint depth		= sideLength;
	Texture3D::Initialize(width, height, depth, typelessFormat, srvFormat, uavFormat, 0, _mipmapCount);

	auto device = Director::SharedInstance()->GetDirectX()->GetDevice();

	for(uint i=0; i<mipmapCount - 1; ++i)
	{
		UnorderedAccessView* uav = new UnorderedAccessView;

		width	/= 2;
		height	/= 2;
		depth	/= 2;

		uav->Initialize(uavFormat, width * height * depth, _texture, D3D11_UAV_DIMENSION_TEXTURE3D, i, depth);

		_mipmapUAVs.push_back(uav);
	}
}

void AnisotropicVoxelMapAtlas::Destroy()
{
	for(auto iter = _mipmapUAVs.begin(); iter != _mipmapUAVs.end(); ++iter)
	{
		(*iter)->Destroy();
		SAFE_DELETE(*iter);
	}
	
	_mipmapUAVs.clear();
	Texture3D::Destory();
}