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

void VoxelConeTracing::Initialize(const Device::DirectX* dx,
								  const Buffer::ConstBuffer* giInfoCB)
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

	std::vector<ShaderForm::InputConstBuffer> icbs;
	{
		icbs.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::VCT_GlobalInfoCB), giInfoCB));
	}
	_shader->SetInputConstBuffers(icbs);

	_indirectColorMap = new RenderTexture;
	_indirectColorMap->Initialize(mapSize,
								  DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 1);
}

void VoxelConeTracing::Destroy()
{
	_indirectColorMap->Destroy();
}

void VoxelConeTracing::Run(const Device::DirectX* dx, const VoxelMap* injectedColorMap, const Camera::MeshCamera* meshCam)
{
	ID3D11DeviceContext* context = dx->GetContext();

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	context->ClearRenderTargetView(_indirectColorMap->GetRenderTargetView(), clearColor);

	auto CSSetShaderResource = [](ID3D11DeviceContext* context, TextureBindIndex bind, const ShaderResourceView* srv) -> void
	{
		ID3D11ShaderResourceView* view = srv ? srv->GetView() : nullptr;
		context->CSSetShaderResources(uint(bind), 1, &view);
	};

	CSSetShaderResource(context, TextureBindIndex::VCT_InputVoxelMap,					injectedColorMap->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Albedo_Occlusion,			meshCam->GetGBufferAlbedoOcclusion()->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_MotionXY_Height_Metallic,	meshCam->GetGBufferMotionXYHeightMetallic()->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Normal_Roughness,			meshCam->GetGBufferNormalRoughness()->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Depth,						meshCam->GetOpaqueDepthBuffer()->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::GBuffer_Emission_Specularity,		meshCam->GetGBufferEmissionSpecularity()->GetShaderResourceView());
	CSSetShaderResource(context, TextureBindIndex::VCT_InputDirectColorMap,				meshCam->GetUncompressedOffScreen()->GetShaderResourceView());

	ID3D11Buffer* tbrParamCB = meshCam->GetTBRParamConstBuffer()->GetBuffer();
	context->CSSetConstantBuffers(uint(ConstBufferBindIndex::TBRParam), 1, &tbrParamCB);

	ID3D11UnorderedAccessView* uav = _indirectColorMap->GetUnorderedAccessView()->GetView();
	context->CSSetUnorderedAccessViews(uint(UAVBindIndex::VCT_OutIndirectMap), 1, &uav, nullptr);

	ID3D11SamplerState* samplerState = dx->GetConeTracingSamplerState();
	context->CSSetSamplers(uint(SamplerStateBindIndex::DefaultSamplerState), 1,	&samplerState);
	_shader->Dispatch(context);

	// Clear
	{
		samplerState = nullptr;
		context->CSSetSamplers(uint(SamplerStateBindIndex::DefaultSamplerState), 1, &samplerState);

		uav = nullptr;
		context->CSSetUnorderedAccessViews(uint(UAVBindIndex::VCT_OutIndirectMap), 1, &uav, nullptr);

		tbrParamCB = nullptr;
		context->CSSetConstantBuffers(uint(ConstBufferBindIndex::TBRParam), 1, &tbrParamCB);

		CSSetShaderResource(context, TextureBindIndex::VCT_InputVoxelMap,				nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Albedo_Occlusion,		nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_MotionXY_Height_Metallic,		nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Normal_Roughness,		nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Emission_Specularity,				nullptr);
		CSSetShaderResource(context, TextureBindIndex::GBuffer_Depth,					nullptr);
		CSSetShaderResource(context, TextureBindIndex::VCT_InputDirectColorMap,			nullptr);
	}
}