#include "ShadowAtlasMapRenderer.h"
#include "LightCullingUtility.h"

using namespace Rendering;
using namespace Rendering::Renderer;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Math;
using namespace Device;

void ShadowAtlasMapRenderer::Initialize(DirectX& dx,
	uint dlMapResolution, uint slMapResolution, uint plMapResolution)
{
	ReSizeShadowMap<SpotLightShadow>(dx, ResizeParam(dlMapResolution, 1));
	ReSizeShadowMap<PointLightShadow>(dx, ResizeParam(plMapResolution, 1));
	ReSizeShadowMap<DirectionalLightShadow>(dx, ResizeParam(slMapResolution, 1));
}

void ShadowAtlasMapRenderer::Destroy()
{
	GetShadowAtlasMap<DirectionalLightShadow>().Destroy();
	GetShadowAtlasMap<SpotLightShadow>().Destroy();
	GetShadowAtlasMap<PointLightShadow>().Destroy();
}
