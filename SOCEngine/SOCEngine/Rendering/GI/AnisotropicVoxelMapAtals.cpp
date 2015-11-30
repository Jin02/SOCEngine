#include "AnisotropicVoxelMapAtlas.h"
#include "Director.h"

using namespace Device;
using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::View;

AnisotropicVoxelMapAtlas::AnisotropicVoxelMapAtlas()
	: Texture3D(), _uavBindIndex(-1)
{
}

AnisotropicVoxelMapAtlas::~AnisotropicVoxelMapAtlas()
{
	Destroy();
}

void AnisotropicVoxelMapAtlas::Initialize(
	uint sideLength, uint maxNumOfCascade, DXGI_FORMAT format, uint mipmapCount, uint uavBindIndex)
{
	const uint bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	_uavBindIndex	= uavBindIndex;
	_sideLength		= sideLength;
	_mipmapCount	= mipmapCount;

	uint width		= sideLength * (uint)Direction::Num;
	uint height		= sideLength * maxNumOfCascade;
	Texture3D::Initialize(width, height, sideLength, format, bindFlags, _mipmapCount);

	auto device = Director::GetInstance()->GetDirectX()->GetDevice();

	for(uint i=0; i<mipmapCount - 1; ++i)
	{
		UnorderedAccessView* uav = new UnorderedAccessView;

		uint halfWidth	= width / 2;
		uint halfHeight	= height / 2;
		uint halfDepth	= sideLength / 2;

		uav->Initialize(format, halfWidth * halfHeight * halfDepth, _texture, D3D11_UAV_DIMENSION_TEXTURE3D, i, halfDepth);

		_mipmapUAVs.push_back(uav);
	}
}

void AnisotropicVoxelMapAtlas::BindUAVsToPixelShader(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context	= dx->GetContext();
	const uint initCounts[1]		= {0xFFFFFFFFu};		
	ID3D11UnorderedAccessView* view	= _uav->GetView();

	context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, _uavBindIndex, 1, &view, initCounts);
}

void AnisotropicVoxelMapAtlas::UnbindUAVs(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();
	const uint initCounts[1] = {0xFFFFFFFFu};		
	ID3D11UnorderedAccessView* view = nullptr;

	context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, _uavBindIndex, 1, &view, initCounts);
}