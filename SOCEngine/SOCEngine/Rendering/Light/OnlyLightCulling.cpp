#include "OnlyLightCulling.h"
#include "EngineShaderFactory.hpp"
#include "LightCullingUtility.h"
#include "MainCamera.h"

using namespace Rendering::Light;
using namespace Rendering::Texture;
using namespace Rendering::Factory;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::Manager;

void OnlyLightCulling::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& renderRectSize)
{
	std::vector<ShaderMacro> macros{	ShaderMacro("USE_ATOMIC", ""),
										ShaderMacro("USE_COMPUTE_SHADER", ""),
										ShaderMacro("ENABLE_BLEND", ""),
										dx.GetMSAAShaderMacro() };

	EngineShaderFactory factory(&shaderMgr);
	_cs = *factory.LoadComputeShader(dx, "OnlyLightCullingCS", "OnlyLightCullingCS", &macros, "@OnlyLightCulling");

	ComputeShader::ThreadGroup threadGroup(0, 0, 0);
	{
		Size<uint> groupSize = CullingUtility::CalcThreadGroupSize(renderRectSize);
		threadGroup.x = groupSize.w; threadGroup.y = groupSize.h; threadGroup.z = 1;
	}
	_cs.SetThreadGroupInfo(threadGroup);

	// Ouput Buffer Setting
	{
		Size<uint> size = CullingUtility::CalcThreadGroupSize(renderRectSize);
		uint num = CullingUtility::CalcMaxNumLightsInTile(renderRectSize) * size.w * size.h;

		_srb.Initialize(dx, 4, num, DXGI_FORMAT_R32_UINT, nullptr, false,  D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);
		_uav.Initialize(dx, DXGI_FORMAT_R32_UINT, num, _srb.GetBaseBuffer().GetBuffer(), D3D11_UAV_DIMENSION_BUFFER);
	}

}

inline void OnlyLightCulling::Dispatch(Device::DirectX & dx, Camera::MainCamera & mainCamera, Manager::LightManager & lightMgr)
{
	ID3D11DeviceContext* context = dx.GetContext();

	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::TBRParam, mainCamera.GetTBRParamCB());
	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::Camera, mainCamera.GetCameraCB());

	auto& plBuffer = lightMgr.GetBuffer<PointLight>().GetLightBuffer();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightRadiusWithCenter, plBuffer.GetTransformSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::PointLightOptionalParamIndex, plBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	auto& slBuffer = lightMgr.GetBuffer<SpotLight>().GetLightBuffer();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightRadiusWithCenter, slBuffer.GetTransformSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightOptionalParamIndex, slBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::SpotLightParam, slBuffer.GetParamSRBuffer().GetShaderResourceView());

	auto& gbuffers = mainCamera.GetGBuffers();
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth, gbuffers.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_BlendedDepth, gbuffers.blendedDepthBuffer.GetTexture2D().GetShaderResourceView());

	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex::Lightculling_LightIndexBuffer, _uav);

	_cs.Dispatch(dx);

	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::Lightculling_LightIndexBuffer);

	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Camera);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightRadiusWithCenter);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::PointLightOptionalParamIndex);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightRadiusWithCenter);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightOptionalParamIndex);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::SpotLightParam);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_BlendedDepth);
}
