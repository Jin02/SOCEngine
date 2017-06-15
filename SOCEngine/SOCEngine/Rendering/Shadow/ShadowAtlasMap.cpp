#include "ShadowAtlasMap.h"
#include <assert.h>

using namespace Rendering::Shadow;
using namespace Math;
using namespace Core;

void ShadowAtlasMap::Initialize(Device::DirectX & dx, uint lightCount, uint resolution, bool isPointLight)
{
	uint capacity = Next2Squre(lightCount);
	auto size = Size<uint>(capacity * resolution, resolution * (isPointLight ? 6 : 1));

	_atlasMap.Initialize(dx, size, true, 1);

	_resolution		= resolution;
	_capacity		= capacity;
	_isPointLight	= isPointLight;
}

void ShadowAtlasMap::ReSize(Device::DirectX & dx, uint lightCount, uint resolution)
{
	if (ChangedAtlasSize(lightCount, resolution) == false)
		return;

	Destroy();
	Initialize(dx, lightCount, resolution, _isPointLight);
}

void ShadowAtlasMap::Destroy()
{
	_atlasMap.Destroy();
}

inline bool ShadowAtlasMap::ChangedAtlasSize(uint lightCount, uint resolution)
{
	uint capacity = Next2Squre(lightCount);
	return ( (_capacity != capacity) | (_resolution != resolution) );
}

inline uint ShadowAtlasMap::Next2Squre(uint value)
{
	return 1 << (uint)(ceil(log((uint)value) / log(2.0f)));
}
