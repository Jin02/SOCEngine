#include "VoxelConeTracing.h"
#include "ShaderFactory.hpp"
#include "ShaderManager.h"
#include "MainRenderer.h"
#include "AutoBinder.hpp"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Math;
using namespace Rendering::Shadow;
using namespace Rendering::PostProcessing;
using namespace Rendering::Factory;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::View;
using namespace Rendering::Shader;
using namespace Rendering::Factory;
using namespace Rendering::Camera;
using namespace Rendering::RenderState;

void VoxelConeTracing::Initialize(DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& renderSize)
{
	ShaderFactory factory(&shaderMgr);
	std::vector<ShaderMacro> macro{dx.GetMSAAShaderMacro()};
	_shader = *factory.LoadComputeShader(dx, "VoxelConeTracing", "VoxelConeTracingCS", &macro, "@VCT");

	_group = ComputeShader::ThreadGroup{(renderSize.w + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES,
										(renderSize.h + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES, 1};
}

void VoxelConeTracing::Run(DirectX& dx, RenderTexture& outIndirectColorMap, const Param&& param)
{
	AutoBinderSRV<ComputeShader> injection(dx,	TextureBindIndex::VCTInjectionSourceColorMap,				param.injectionSourceMap.GetTexture3D().GetShaderResourceView());
	AutoBinderSRV<ComputeShader> mipmapped(dx,	TextureBindIndex::VCTMipmappedInjectionColorMap,			param.mipmappedInjectionMap.GetTexture3D().GetShaderResourceView());

	auto& gbuffer = param.mainSystem.renderer.GetGBuffers();
	AutoBinderSRV<ComputeShader> albedo(dx,		TextureBindIndex::GBuffer_Albedo_Occlusion,					gbuffer.albedo_occlusion.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<ComputeShader> velocity(dx,	TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	gbuffer.velocity_metallic_specularity.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<ComputeShader> normal(dx,		TextureBindIndex::GBuffer_Normal_Roughness,					gbuffer.normal_roughness.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<ComputeShader> depth(dx,		TextureBindIndex::GBuffer_Depth,							gbuffer.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
	AutoBinderSRV<ComputeShader> emission(dx,	TextureBindIndex::GBuffer_Emission_MaterialFlag,			gbuffer.emission_materialFlag.GetTexture2D()->GetShaderResourceView());

	AutoBinderCB<ComputeShader> tbr(dx,			ConstBufferBindIndex::TBRParam,								param.mainSystem.renderer.GetTBRParamCB());
	AutoBinderCB<ComputeShader> cam(dx,			ConstBufferBindIndex::Camera,								param.mainSystem.camera.GetCameraCB());
	AutoBinderCB<ComputeShader> vxgiStatic(dx,	ConstBufferBindIndex::VXGIStaticInfoCB,						param.infoCB.staticInfoCB);
	AutoBinderCB<ComputeShader> vxgiDynamic(dx,	ConstBufferBindIndex::VXGIDynamicInfoCB,					param.infoCB.dynamicInfoCB);

	AutoBinderSampler<ComputeShader> sampler(dx,SamplerStateBindIndex::DefaultSamplerState,					SamplerState::ConeTracingLinear);
	AutoBinderUAV output(dx,					UAVBindIndex::VCTOutIndirectMap,							outIndirectColorMap.GetTexture2D()->GetUnorderedAccessView());

	_shader.Dispatch(dx, _group);
}
