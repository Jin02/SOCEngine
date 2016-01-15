#include "GlobalIllumination.h"
#include "BindIndexInfo.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;

GlobalIllumination::GlobalIllumination()
	: _giGlobalInfoCB(nullptr)
{
}

GlobalIllumination::~GlobalIllumination()
{
	SAFE_DELETE(_giGlobalInfoCB);
}

void GlobalIllumination::Initialize(uint dimension, float minWorldSize)
{
	_giGlobalInfoCB = new ConstBuffer;
	_giGlobalInfoCB->Initialize(sizeof(GlobalInfo));

	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

	const uint mipmapLevels = min((uint)Log2((float)dimension) + 1, 1);
	
	GlobalInfo globalInfo;
	globalInfo.maxNumOfCascade		= 1;
	globalInfo.voxelDimensionPow2	= (uint)Log2((float)dimension);
	globalInfo.initVoxelSize		= minWorldSize / (float)dimension;
	globalInfo.initWorldSize		= minWorldSize;
	globalInfo.maxMipLevel			= (float)mipmapLevels;
}