#include "GlobalIllumination.h"

using namespace Device;
using namespace Rendering;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::GI;
using namespace Rendering::Texture;

void GlobalIllumination::Initialize(DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& renderSize, const GIInitParam& giParam)
{
	if (giParam.GetUseVXGI())
	{
		VXGIStaticInfo staticInfo(giParam.vxgiParam.voxelizeDimension, giParam.vxgiParam.voxelizeSize);
		_vxgi.Initialize(dx, shaderMgr, renderSize, staticInfo);
	}
}

void GlobalIllumination::Run(DirectX& dx, const VXGI::Param&& param)
{
	// TODO :	����� In-door GI�� ó�� ��
	//			���߿� �ð��� ���ٸ�(?) Out-door ���� ó���Ѵ�
	if(_vxgi.GetDirty() | param.lightMgr.GetHasChangedLightCount())
	{
		_vxgi.SetPackedNumfOfLights(param.lightMgr.GetPackedLightCount());
		_vxgi.UpdateGIDynamicInfoCB(dx);
	}

	_vxgi.Run(dx, std::move(param));
}
