#include "VoxelConeTracing.h"
#include "Utility.h"
#include "EngineShaderFactory.hpp"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "Director.h"

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
using namespace GPGPU::DirectCompute;
using namespace Resource;


VoxelConeTracing::VoxelConeTracing() : _shader(nullptr), _indirectColorMap(nullptr)
{
#if defined(USE_GAUSSIAN_BLUR) || defined(USE_BILATERAL_FILTERING)
	_blur = nullptr;
#endif
}

VoxelConeTracing::~VoxelConeTracing()
{
	Destroy();

	SAFE_DELETE(_shader);
	SAFE_DELETE(_indirectColorMap);
#if defined(USE_GAUSSIAN_BLUR) || defined(USE_BILATERAL_FILTERING)
	SAFE_DELETE(_blur);
#endif
}

void VoxelConeTracing::Initialize(const Device::DirectX* dx)
{
	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, "VoxelConeTracing");

	ASSERT_MSG_IF(filePath.empty() == false, "Error, File path is empty!");

	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	// Compile Compute Shader
	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "VoxelConeTracingCS", false, nullptr);

	ComputeShader::ThreadGroup threadGroup(0, 0, 0);
	Size<uint> mapSize = dx->GetBackBufferSize();
	{
		threadGroup.x = (mapSize.w + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES;
		threadGroup.y = (mapSize.h + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES;
		threadGroup.z = 1;
	}

	_shader = new ComputeShader(threadGroup, blob);
	ASSERT_MSG_IF(_shader->Initialize(), "can not create compute shader");

	_indirectColorMap = new RenderTexture;
	_indirectColorMap->Initialize(mapSize,
								  DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 1);

#if defined(USE_GAUSSIAN_BLUR)
	_blur = new GaussianBlur;
	_blur->Initialize(dx, mapSize, DXGI_FORMAT_R8G8B8A8_UNORM);
#elif defined(USE_BILATERAL_FILTERING)
	_blur = new BilateralFiltering;
	_blur->Initialize(BilateralFiltering::Type::Near, mapSize, DXGI_FORMAT_R8G8B8A8_UNORM);
#endif
}

void VoxelConeTracing::Destroy()
{
	_indirectColorMap->Destroy();
#if defined(USE_GAUSSIAN_BLUR) || defined(USE_BILATERAL_FILTERING)
	_blur->Destroy();
#endif
}

void VoxelConeTracing::Run(const Device::DirectX* dx, const VoxelMap* injectionSourceMap, const VoxelMap* mipmappedInjectionMap, const Camera::MeshCamera* meshCam,
							const Buffer::ConstBuffer* vxgiStaticInfoCB, const Buffer::ConstBuffer* vxgiDynamicInfoCB)
{
	ID3D11DeviceContext* context = dx->GetContext();

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	context->ClearRenderTargetView(_indirectColorMap->GetRenderTargetView(), clearColor);

	ComputeShader::BindTexture(context, 			TextureBindIndex::VCTInjectionSourceColorMap,				injectionSourceMap);
	ComputeShader::BindTexture(context, 			TextureBindIndex::VCTMipmappedInjectionColorMap,			mipmappedInjectionMap);

	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Albedo_Occlusion,					meshCam->GetGBufferAlbedoOcclusion());
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	meshCam->GetGBufferVelocityMetallicSpecularity());
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Normal_Roughness,					meshCam->GetGBufferNormalRoughness());
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Depth,							meshCam->GetOpaqueDepthBuffer());
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Emission_MaterialFlag,			meshCam->GetGBufferEmissionMaterialFlag());

	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,								meshCam->GetTBRParamConstBuffer());
	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,								meshCam->GetCameraConstBuffer());
	ComputeShader::BindSamplerState(context,		SamplerStateBindIndex::DefaultSamplerState,					dx->GetConeTracingSamplerState());
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VCTOutIndirectMap,							_indirectColorMap->GetUnorderedAccessView());

	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::VXGIStaticInfoCB,						vxgiStaticInfoCB);
	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::VXGIDynamicInfoCB,					vxgiDynamicInfoCB);

	_shader->Dispatch(context);

	// Clear
	ComputeShader::BindTexture(context, 			TextureBindIndex::VCTInjectionSourceColorMap,				nullptr);
	ComputeShader::BindTexture(context, 			TextureBindIndex::VCTMipmappedInjectionColorMap,			nullptr);

	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Albedo_Occlusion,					nullptr);
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	nullptr);
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Normal_Roughness,					nullptr);
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Depth,							nullptr);
	ComputeShader::BindTexture(context, 			TextureBindIndex::GBuffer_Emission_MaterialFlag,			nullptr);

	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::TBRParam,								nullptr);
	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::Camera,								nullptr);

	ComputeShader::BindSamplerState(context,		SamplerStateBindIndex::DefaultSamplerState,					nullptr);
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VCTOutIndirectMap,							nullptr);

	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::VXGIStaticInfoCB,						nullptr);
	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::VXGIDynamicInfoCB,					nullptr);


#if defined(USE_GAUSSIAN_BLUR)
	_blur->Render(dx, _indirectColorMap, _indirectColorMap);
#elif defined(USE_BILATERAL_FILTERING)
	_blur->Render(dx, _indirectColorMap, meshCam->GetOpaqueDepthBuffer(), _indirectColorMap);
#endif
}
