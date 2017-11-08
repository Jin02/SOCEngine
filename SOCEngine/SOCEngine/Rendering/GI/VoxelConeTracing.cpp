#include "VoxelConeTracing.h"
#include "ShaderFactory.hpp"
#include "ShaderManager.h"

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

	ComputeShader::ThreadGroup threadGroup(0, 0, 0);
	Size<uint> mapSize = dx.GetBackBufferSize().Cast<uint>();
	{
		threadGroup.x = (mapSize.w + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES;
		threadGroup.y = (mapSize.h + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES;
		threadGroup.z = 1;
	}
	_shader.SetThreadGroupInfo(threadGroup);

	_indirectColorMap.Initialize(dx, mapSize,
								  DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 1);
}

void VoxelConeTracing::Run(DirectX& dx, VoxelMap& injectionSourceMap, VoxelMap& mipmappedInjectionMap, VXGIInfoCB& infoCB, MainRenderingSystemParam& mainSystem)
{
	_indirectColorMap.Clear(dx, Color::Clear());

	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::VCTInjectionSourceColorMap,				injectionSourceMap.GetTexture3D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::VCTMipmappedInjectionColorMap,			mipmappedInjectionMap.GetTexture3D().GetShaderResourceView());

	auto& gbuffer = mainSystem.renderer.GetGBuffers();
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Albedo_Occlusion,					gbuffer.albedo_occlusion.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	gbuffer.velocity_metallic_specularity.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Normal_Roughness,					gbuffer.normal_roughness.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Depth,							gbuffer.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
	ComputeShader::BindShaderResourceView(dx,	TextureBindIndex::GBuffer_Emission_MaterialFlag,			gbuffer.emission_materialFlag.GetTexture2D().GetShaderResourceView());

	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::TBRParam,								mainSystem.renderer.GetTBRParamCB());
	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::Camera,								mainSystem.camera.GetCameraCB());

	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::VXGIStaticInfoCB,						infoCB.staticInfoCB);
	ComputeShader::BindConstBuffer(dx,			ConstBufferBindIndex::VXGIDynamicInfoCB,					infoCB.dynamicInfoCB);

	ComputeShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::ConeTracing);

	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex::VCTOutIndirectMap, _indirectColorMap.GetTexture2D().GetUnorderedAccessView());

	_shader.Dispatch(dx);

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
