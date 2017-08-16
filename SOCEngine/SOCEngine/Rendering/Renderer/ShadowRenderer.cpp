#include "ShadowRenderer.h"
#include "LightCullingUtility.h"

using namespace Rendering;
using namespace Rendering::Renderer;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Math;
using namespace Device;

void ShadowRenderer::Initialize(DirectX & dx,
	uint dlMapResolution, uint slMapResolution, uint plMapResolution)
{	
	ReSizeShadowMap<SpotLightShadow>(dx, ReSizeParam(dlMapResolution, 1), false);
	ReSizeShadowMap<PointLightShadow>(dx, ReSizeParam(plMapResolution, 1), false);
	ReSizeShadowMap<DirectionalLightShadow>(dx, ReSizeParam(slMapResolution, 1), false);
}

void ShadowRenderer::Destroy()
{
	GetShadowAtlasMap<DirectionalLightShadow>().Destroy();
	GetShadowAtlasMap<SpotLightShadow>().Destroy();
	GetShadowAtlasMap<PointLightShadow>().Destroy();
}
