#include "DeferredShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"

using namespace Rendering::Light;

DeferredShadingWithLightCulling::DeferredShadingWithLightCulling()
{
}

DeferredShadingWithLightCulling::~DeferredShadingWithLightCulling()
{
}

void DeferredShadingWithLightCulling::Initialize()
{

	EnableAlphaBlend(true); //Default
}

void DeferredShadingWithLightCulling::EnableAlphaBlend(bool enable)
{
	std::string filePath;
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "TileBasedDeferredShading");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	LightCulling::Initialize(filePath, "CS", enable);
}

