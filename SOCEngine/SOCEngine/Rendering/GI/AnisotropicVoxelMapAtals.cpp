#include "AnisotropicVoxelMapAtlas.h"

using namespace Math;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;

AnisotropicVoxelMapAtlas::AnisotropicVoxelMapAtlas()
	: _voxelMapAtlas(nullptr), _uavBindIndex(-1)
{
}

AnisotropicVoxelMapAtlas::~AnisotropicVoxelMapAtlas()
{
	Destroy();

	SAFE_DELETE(_voxelMapAtlas);
}

void AnisotropicVoxelMapAtlas::Initialize(
	uint sideLength, uint maxNumOfCascade, DXGI_FORMAT format, uint mipmapLevels, uint uavBindIndex)
{
	_uavBindIndex	= uavBindIndex;
	_sideLength		= sideLength;
	_mipmapLevels	= mipmapLevels;

	_voxelMapAtlas	= new VoxelMap;

	uint width		= sideLength * (uint)Direction::Num;
	uint height		= sideLength * maxNumOfCascade;
	_voxelMapAtlas->Initialize(width, height, sideLength, format);
}

void AnisotropicVoxelMapAtlas::Destroy()
{
	_voxelMapAtlas->Destroy();
}

void AnisotropicVoxelMapAtlas::BindUAVsToPixelShader(const Device::DirectX* dx)
{
	//ID3D11DeviceContext* context	= dx->GetContext();
	//const uint initCounts[1]		= {0xFFFFFFFFu};		
	//ID3D11UnorderedAccessView* view	= _voxelMapAtlas->GetUnorderedAccessView()->GetView();

	//context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, _uavBindIndex, 1, &view, initCounts);
}

void AnisotropicVoxelMapAtlas::UnbindUAVs(const Device::DirectX* dx)
{
	//ID3D11DeviceContext* context = dx->GetContext();
	//const uint initCounts[1] = {0xFFFFFFFFu};		
	//ID3D11UnorderedAccessView* view = nullptr;

	//context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, _uavBindIndex, 1, &view, initCounts);
}