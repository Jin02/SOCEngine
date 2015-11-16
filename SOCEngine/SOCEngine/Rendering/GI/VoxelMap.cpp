#include "VoxelMap.h"

using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;

VoxelMap::VoxelMap()
	: Texture3D(nullptr, nullptr), _uav(nullptr)
{
}

VoxelMap::~VoxelMap()
{
	Destroy();
}

void VoxelMap::Initialize(const Math::Size<unsigned int>& size, uint depth, DXGI_FORMAT format)
{
	const uint mipLevels = 1;
	const uint bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;

	Texture3D::Initialize(size, depth, format, bindFlags, mipLevels);

	_uav = new UnorderedAccessView;
	_uav->Initialize(format, size.w * size.h * depth, _texture, UnorderedAccessView::Type::Texture);
}

void VoxelMap::Destroy()
{
	SAFE_DELETE(_uav);
}