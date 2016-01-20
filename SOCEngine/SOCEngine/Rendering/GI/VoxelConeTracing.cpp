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
using namespace Rendering::Factory;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::View;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Resource;

VoxelConeTracing::VoxelConeTracing()
	: _shader(nullptr), _indirectColorMap(nullptr)
{
}

VoxelConeTracing::~VoxelConeTracing()
{
	Destroy();

	SAFE_DELETE(_shader);
	SAFE_DELETE(_indirectColorMap);
}

void VoxelConeTracing::Initialize(const Device::DirectX* dx)
{
	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, "VoxelConeTracing");

	ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	// Compile Compute Shader
	ID3DBlob* blob = nullptr;
	{
		std::vector<ShaderMacro> macros;
		macros.push_back(dx->GetMSAAShaderMacro());
	
		blob = shaderMgr->CreateBlob(filePath, "cs", "VoxelConeTracingCS", false, &macros);
	}

	ComputeShader::ThreadGroup threadGroup(0, 0, 0);
	Size<uint> mapSize(0, 0);
	{
		// Compute MapSize
		{
			mapSize = dx->GetBackBufferSize();
			if(dx->GetMSAADesc().Count > 1)
			{
				mapSize.w *= 2;
				mapSize.h *= 2;
			}
		}

		threadGroup.x = (mapSize.w + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES;
		threadGroup.y = (mapSize.h + VOXEL_CONE_TRACING_TILE_RES - 1) / VOXEL_CONE_TRACING_TILE_RES;
		threadGroup.z = 1;
	}

	_shader = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");

	_indirectColorMap = new RenderTexture;
	_indirectColorMap->Initialize(mapSize,
								  DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 1);
}

void VoxelConeTracing::Destroy()
{
	_indirectColorMap->Destroy();
}

void VoxelConeTracing::Run(const Device::DirectX* dx,
						   const AnisotropicVoxelMapAtlas* mipmappedVoxelColorMap,
						   const DirectLightingParam& param)
{
	ID3D11DeviceContext* context = dx->GetContext();

	auto CSSetShaderResource = [](ID3D11DeviceContext* context, TextureBindIndex bind, const ShaderResourceView* srv) -> void
	{
		ID3D11ShaderResourceView* view = srv ? srv->GetView() : nullptr;
		context->CSSetShaderResources(uint(bind), 1, &view);
	};

	CSSetShaderResource(context, TextureBindIndex::VCT_InputVoxelColorMap,			mipmappedVoxelColorMap->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Albedo_Emission,			param.gbuffer.albedo_emission->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Specular_Metallic,		param.gbuffer.specular_metallic->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Normal_Roughness,		param.gbuffer.normal_roughness->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Depth,					param.opaqueDepthBuffer->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::VCT_InputDirectColorMap,			param.directLightingColorMap->GetShaderResourceView());

	ID3D11UnorderedAccessView* uav = _indirectColorMap->GetUnorderedAccessView()->GetView();
	context->CSGetUnorderedAccessViews(uint(UAVBindIndex::VCT_OutIndirectMap), 1,	&uav);

	ID3D11SamplerState* samplerState = dx->GetSamplerStateLinear();
	context->CSSetSamplers(uint(SamplerStateBindIndex::DefaultSamplerState), 1,		&samplerState);
	_shader->Dispatch(context);

	// Clear
	{
		samplerState = nullptr;
		context->CSSetSamplers(uint(SamplerStateBindIndex::DefaultSamplerState), 1,		&samplerState);

		uav = nullptr;
		context->CSGetUnorderedAccessViews(uint(UAVBindIndex::VCT_OutIndirectMap), 1,	&uav);

		CSSetShaderResource(context, TextureBindIndex::VCT_InputVoxelColorMap,			nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Albedo_Emission,			nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Specular_Metallic,		nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Normal_Roughness,		nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Depth,					nullptr);
		CSSetShaderResource(context, TextureBindIndex::VCT_InputDirectColorMap,			nullptr);
	}
}