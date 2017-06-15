#include "ShadowRenderer.h"
#include "LightCullingUtility.h"

using namespace Rendering;
using namespace Rendering::Renderer;
using namespace Rendering::Light;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Math;
using namespace Device;

void ShadowRenderer::Initialize(DirectX & dx)
{	
	GetShadowAtlasMap<DirectionalLightShadow>().Initialize(dx, ShadowManager::DirectionalLightInitCount, 512, false);
	GetShadowAtlasMap<SpotLightShadow>().Initialize(dx, ShadowManager::SpotLightInitCount, 512, false);
	GetShadowAtlasMap<PointLightShadow>().Initialize(dx, ShadowManager::PointLightInitCount, 512, true);
}

void ShadowRenderer::Destroy()
{
	GetShadowAtlasMap<DirectionalLightShadow>().Destroy();
	GetShadowAtlasMap<SpotLightShadow>().Destroy();
	GetShadowAtlasMap<PointLightShadow>().Destroy();
}
