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

void AnisotropicVoxelMap::BindUAVsToPixelShader(const Device::DirectX* dx, const uint bindSlotOffset)
{
	ID3D11DeviceContext* context = dx->GetContext();
	const uint initCounts[1] = {0xFFFFFFFFu};		

	for(uint i=0; i<(uint)Direction::Num; ++i)
	{
		uint bindSlotIndex = bindSlotOffset + i;
		const UnorderedAccessView* uav = _voxelMaps[i]->GetUnorderedAccessView();
		ID3D11UnorderedAccessView* view = uav->GetView();

		context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, bindSlotIndex, 1, &view, initCounts);
	}
}

void AnisotropicVoxelMap::UnbindUAVs(const Device::DirectX* dx, const uint bindSlotOffset)
{
	ID3D11DeviceContext* context = dx->GetContext();
	const uint initCounts[1] = {0xFFFFFFFFu};		

	for(uint i=0; i<(uint)Direction::Num; ++i)
	{
		uint bindSlotIndex = bindSlotOffset + i;
		ID3D11UnorderedAccessView* view = nullptr;

		context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, bindSlotIndex, 1, &view, initCounts);
	}
}

void AnisotropicVoxelMap::Clear(const Device::DirectX* dx)
{
	ID3D11DeviceContext* context = dx->GetContext();
	float clearValues[4] = {0.0f, };

	for(uint i=0; i<(uint)Direction::Num; ++i)
	{
		const UnorderedAccessView* uav = _voxelMaps[i]->GetUnorderedAccessView();
		context->ClearUnorderedAccessViewFloat(uav->GetView(), clearValues);
	}
}