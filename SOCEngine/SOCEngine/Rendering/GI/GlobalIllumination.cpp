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

void GlobalIllumination::Initialize()
{
	_giGlobalInfoCB = new ConstBuffer;
	_giGlobalInfoCB->Initialize(sizeof(GlobalInfo));
}