#include "OnlyLightCulling.h"
#include "ShaderFactory.hpp"
#include "LightCullingUtility.h"

using namespace Rendering::Light;
using namespace Rendering::Texture;
using namespace Rendering::Factory;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::Manager;
using namespace Rendering::Renderer;

void OnlyLightCulling::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& maxRenderRectSize)
{
	std::vector<ShaderMacro> macros{	ShaderMacro("USE_ATOMIC", ""),
										ShaderMacro("USE_COMPUTE_SHADER", ""),
										ShaderMacro("ENABLE_BLEND", ""),
										dx.GetMSAAShaderMacro() };

	_cs = *ShaderFactory::LoadComputeShader(dx, shaderMgr, "OnlyLightCullingCS", "OnlyLightCullingCS", &macros, "@OnlyLightCulling");

	// Ouput Buffer Setting
	{
		Size<uint> size	= CullingUtility::CalcThreadGroupSize(maxRenderRectSize);
		uint num		= CullingUtility::CalcMaxNumLightsInTile(maxRenderRectSize) * size.w * size.h;

		_srb.Initialize(dx, 4, num, DXGI_FORMAT_R32_UINT, nullptr,  D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);
		_uav.Initialize(dx, DXGI_FORMAT_R32_UINT, num, _srb.GetBaseBuffer().GetBuffer(), D3D11_UAV_DIMENSION_BUFFER);
	}
}

void OnlyLightCulling::Destroy()
{
	_srb.Destroy();
	_uav.Destroy();
}

void OnlyLightCulling::Dispatch(Device::DirectX& dx, const ComputeShader::ThreadGroup& group)
{
	ComputeShader::BindUnorderedAccessView(dx,	UAVBindIndex::Lightculling_LightIndexBuffer, _uav);

	_cs.Dispatch(dx, std::move(group));

	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::Lightculling_LightIndexBuffer);
}
