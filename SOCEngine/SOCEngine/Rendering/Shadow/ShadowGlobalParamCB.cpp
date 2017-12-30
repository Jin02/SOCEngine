#include "ShadowGlobalParamCB.h"
#include "ShadowManager.h"
#include "ShadowAtlasMapRenderer.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Manager;
using namespace Rendering::Renderer;
using namespace Rendering::Shader;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Shadow::Buffer;

void ShadowGlobalParamCB::UpdateSubResource(DirectX& dx, const ShadowManager& manager, const ShadowAtlasMapRenderer& renderer)
{
	bool hasChangedCount	= manager.GetHasChangedShadowCount();
	bool hasChangedMapSize	= renderer.GetHasChangedAtlasMapSize();

	if( (hasChangedCount == false) & (hasChangedMapSize == false) )
		return;

	ShadowGlobalParamCBData param;
	{
		auto& dlsBufferObj = manager.GetBuffer<DirectionalLightShadow>();
		auto& plsBufferObj = manager.GetBuffer<PointLightShadow>();
		auto& slsBufferObj = manager.GetBuffer<SpotLightShadow>();

		auto Pack = [](uint d, uint s, uint p) -> uint
		{
			return ((p & 0x7ff) << 21) | ((s & 0x7ff) << 10) | (d & 0x3ff);
		};
		auto Log2Uint = [](uint i) -> uint
		{
			return static_cast<uint>(log(static_cast<float>(i)) / log(2.0f));
		};

		const auto& dlAtlasMap = renderer.GetShadowAtlasMap<DirectionalLightShadow>();
		const auto& plAtlasMap = renderer.GetShadowAtlasMap<PointLightShadow>();
		const auto& slAtlasMap = renderer.GetShadowAtlasMap<SpotLightShadow>();

		param.packedNumOfShadowAtlasCapacity	= Pack(	dlAtlasMap.GetCapacity(), plAtlasMap.GetCapacity(), slAtlasMap.GetCapacity());
		param.packedPowerOfTwoShadowResolution	= Pack(	Log2Uint(dlAtlasMap.GetResolution()),
			Log2Uint(plAtlasMap.GetResolution()),
			Log2Uint(slAtlasMap.GetResolution())	);

		param.packedNumOfShadows				= Pack(	manager.GetPool<DirectionalLightShadow>().GetSize(),
														manager.GetPool<SpotLightShadow>().GetSize(),
														manager.GetPool<PointLightShadow>().GetSize()		);
		param.dummy = 0;
	}

	UpdateSubResource(dx, param);
}