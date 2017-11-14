#include "VoxelConeTracing.h"
#include "ShaderFactory.hpp"
#include "ShaderManager.h"
#include "MainRenderer.h"

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
using namespace Rendering::RenderState;

void VoxelConeTracing::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	ShaderFactory factory(&shaderMgr);
	_shader = *factory.LoadComputeShader(dx, "VoxelConeTracing", "VoxelConeTracingCS", nullptr, "@VCT");

	Size<uint> mapSize = dx.GetBackBufferSize().Cast<uint>();
	_group = ComputeShader::ThreadGroup{(mapSize.w + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES,
										(mapSize.h + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES, 1};
}

void VoxelConeTracing::Run(DirectX& dx, RenderTexture& outIndirectColorMap, const Param&& param)
{
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::VCTInjectionSourceColorMap,				param.injectionSourceMap.GetTexture3D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::VCTMipmappedInjectionColorMap,			param.mipmappedInjectionMap.GetTexture3D().GetShaderResourceView());

	auto& gbuffer = param.mainSystem.renderer.GetGBuffers();
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Albedo_Occlusion,					gbuffer.albedo_occlusion.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	gbuffer.velocity_metallic_specularity.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Normal_Roughness,					gbuffer.normal_roughness.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Depth,							gbuffer.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Emission_MaterialFlag,			gbuffer.emission_materialFlag.GetTexture2D().GetShaderResourceView());

	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::TBRParam,								param.mainSystem.renderer.GetTBRParamCB());
	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::Camera,								param.mainSystem.camera.GetCameraCB());

	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::VXGIStaticInfoCB,						param.infoCB.staticInfoCB);
	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::VXGIDynamicInfoCB,					param.infoCB.dynamicInfoCB);

	ComputeShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::ConeTracing);

	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex::VCTOutIndirectMap, outIndirectColorMap.GetTexture2D().GetUnorderedAccessView());

	_shader.Dispatch(dx, _group);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::VCTInjectionSourceColorMap);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::VCTMipmappedInjectionColorMap);

	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Albedo_Occlusion);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Velocity_Metallic_Specularity);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Normal_Roughness);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth);
	ComputeShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Emission_MaterialFlag);

	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Camera);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIStaticInfoCB);
	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIDynamicInfoCB);

	ComputeShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);

	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::VCTOutIndirectMap);
}
