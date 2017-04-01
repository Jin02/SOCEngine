#include "SkyBox.h"
#include "BindIndexInfo.h"
#include <math.h>

using namespace Rendering;
using namespace Rendering::Sky;
using namespace Rendering::Shader;
using namespace Core;
using namespace Device;
using namespace Math;
using namespace Rendering::Manager;
using namespace Rendering::Texture;

void SkyBox::Initialize(Device::DirectX& dx, Manager::BufferManager& bufferMgr, const std::string& skyBoxMaterialKey)
{
	_skyBoxMaterialKey = skyBoxMaterialKey;
	_base.Initialize(dx, bufferMgr);
}