#include "GlobalIllumination.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::GI;
using namespace Rendering::Texture;

void GlobalIllumination::Initialize(DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& renderSize, const InitParam&& giParam)
{
	_vxgi.Initialize(dx, shaderMgr, renderSize, std::move(giParam.vxgiInitParam));
}

RenderTexture& GlobalIllumination::Run(DirectX& dx, const VXGI::Param&& param)
{
	// TODO :	현재는 In-door GI만 처리 중
	//			나중에 시간이 난다면(?) Out-door 까지 처리한다
	if(_vxgi.GetDirty() == false)
		_vxgi.UpdateGIDynamicInfoCB(dx);

	_vxgi.Run(dx, std::move(param));
	return _vxgi.GetIndirectColorMap();
}
