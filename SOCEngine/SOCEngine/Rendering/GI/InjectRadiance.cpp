#include "InjectRadiance.h"
#include "EngineShaderFactory.hpp"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Shadow;
using namespace Rendering::Factory;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Resource;

InjectRadiance::InjectRadiance() : _shader(nullptr)
{
}

InjectRadiance::~InjectRadiance()
{
	SAFE_DELETE(_shader);
}

void InjectRadiance::Initialize(const std::string& fileName)
{
	std::string filePath = "";
	EngineFactory pathFinder(nullptr);
	pathFinder.FetchShaderFullPath(filePath, fileName);

	ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty!");

	ResourceManager* resourceManager = ResourceManager::SharedInstance();
	auto shaderMgr = resourceManager->GetShaderManager();

	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "CS", false, nullptr);
	_shader = new ComputeShader(ComputeShader::ThreadGroup(0, 0, 0), blob);
	ASSERT_COND_MSG(_shader->Initialize(), "can not create compute shader");
}

void InjectRadiance::Dispath(const Device::DirectX* dx, const DispatchParam& param)
{
	ID3D11DeviceContext* context = dx->GetContext();
	ComputeShader::BindSamplerState(context, SamplerStateBindIndex::ShadowComprisonSamplerState, dx->GetShadowGreaterEqualSamplerComparisonState());

	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::VXGIStaticInfoCB,		param.global.vxgiStaticInfo);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::VXGIDynamicInfoCB,	param.global.vxgiDynamicInfo);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::ShadowGlobalParam,	param.shadowGlobalInfo);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::VoxelizationInfoCB,	param.voxelization.InfoCB);

	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::OutVoxelColorMap, param.OutVoxelColorMap);

	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(1), param.voxelization.AlbedoRawBuffer->GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(2), param.voxelization.NormalRawBuffer->GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(3), param.voxelization.EmissionRawBuffer->GetUnorderedAccessView());

	_shader->Dispatch(context);

	ComputeShader::BindSamplerState(context, SamplerStateBindIndex::ShadowComprisonSamplerState,	nullptr);

	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::VXGIStaticInfoCB,					nullptr);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::VXGIDynamicInfoCB,				nullptr);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::ShadowGlobalParam,				nullptr);
	ComputeShader::BindConstBuffer(context, ConstBufferBindIndex::VoxelizationInfoCB,				nullptr);

	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::OutVoxelColorMap,	nullptr);

	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(1),		nullptr);
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(2),		nullptr);
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex(3),		nullptr);
}

void InjectRadiance::Destroy()
{
}
