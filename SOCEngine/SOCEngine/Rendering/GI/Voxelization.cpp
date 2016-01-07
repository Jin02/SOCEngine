#include "BindIndexInfo.h"
#include "Voxelization.h"
#include "Object.h"
#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

using namespace Resource;
using namespace Core;
using namespace Math;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Camera;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::View;
using namespace GPGPU::DirectCompute;

Voxelization::Voxelization()
:	_infoConstBuffer(nullptr), _clearVoxelMapCS(nullptr), 
	_voxelAlbedoMapAtlas(nullptr), _voxelNormalMapAtlas(nullptr), _voxelEmissionMapAtlas(nullptr)
{
}

Voxelization::~Voxelization()
{
	Destroy();

	SAFE_DELETE(_voxelAlbedoMapAtlas);
	SAFE_DELETE(_voxelNormalMapAtlas);
	SAFE_DELETE(_voxelEmissionMapAtlas);

	SAFE_DELETE(_infoConstBuffer);
	SAFE_DELETE(_clearVoxelMapCS);
}

void Voxelization::Initialize(uint maxNumOfCascade, GlobalInfo& outGlobalInfo, float minWorldSize, uint dimension)
{
	ASSERT_COND_MSG(maxNumOfCascade != 0, "Error, voxelization cascade num is zero.");

	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

	const uint mipmapLevels = min((uint)Log2((float)dimension) + 1, 1);
	
	GlobalInfo& globalInfo = outGlobalInfo;
	globalInfo.maxNumOfCascade		= maxNumOfCascade;
	globalInfo.voxelDimensionPow2	= (uint)Log2((float)dimension);
	globalInfo.initVoxelSize		= minWorldSize / (float)dimension;
	globalInfo.initWorldSize		= minWorldSize;
	globalInfo.maxMipLevel			= (float)mipmapLevels;

	_voxelAlbedoMapAtlas = new AnisotropicVoxelMapAtlas;
	_voxelAlbedoMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels);

	_voxelNormalMapAtlas = new AnisotropicVoxelMapAtlas;
	_voxelNormalMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_UINT, mipmapLevels);

	_voxelEmissionMapAtlas = new AnisotropicVoxelMapAtlas;
	_voxelEmissionMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels);

	_infoConstBuffer = new ConstBuffer;
	_infoConstBuffer->Initialize(sizeof(InfoCBData));
	_constBuffers.push_back(ShaderForm::InputConstBuffer((uint)ConstBufferBindIndex::Voxelization_InfoCB, _infoConstBuffer, false, true, false, true));

	InitializeClearVoxelMap(dimension, maxNumOfCascade);
}

void Voxelization::InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade)
{
	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "ClearVoxelMap");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	ShaderManager* shaderMgr = ResourceManager::SharedInstance()->GetShaderManager();
	ID3DBlob* blob = shaderMgr->CreateBlob(filePath, "cs", "ClearVoxelMapCS", false, nullptr);

	ComputeShader::ThreadGroup threadGroup;
	{
		auto ComputeThreadGroupSideLength = [](uint sideLength)
		{
			return (uint)((float)(sideLength + 8 - 1) / 8.0f);
		};

		threadGroup.x = ComputeThreadGroupSideLength(dimension * (uint)AnisotropicVoxelMapAtlas::Direction::Num);
		threadGroup.y = ComputeThreadGroupSideLength(dimension * maxNumOfCascade);
		threadGroup.z = ComputeThreadGroupSideLength(dimension);
	}

	_clearVoxelMapCS = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_clearVoxelMapCS->Initialize(), "Error, Can't Init ClearVoxelMapCS");

	std::vector<ShaderForm::InputTexture> inputTextures;
	{
		ShaderForm::InputTexture inputTexture;
		inputTexture.bindIndex	= (uint)UAVBindIndex::VoxelMap_Albedo;
		inputTexture.texture	= _voxelAlbedoMapAtlas;
		inputTexture.bindIndex	= (uint)UAVBindIndex::VoxelMap_Normal;
		inputTexture.texture	= _voxelNormalMapAtlas;
		inputTexture.bindIndex	= (uint)UAVBindIndex::VoxelMap_Emission;
		inputTexture.texture	= _voxelEmissionMapAtlas;

		inputTextures.push_back(inputTexture);
	}

	std::vector<ComputeShader::Output> outputs;
	{
		ComputeShader::Output output;
		output.bindIndex	= (uint)UAVBindIndex::VoxelMap_Albedo;
		output.output		= _voxelAlbedoMapAtlas->GetSourceMapUAV();
		output.bindIndex	= (uint)UAVBindIndex::VoxelMap_Normal;
		output.output		= _voxelNormalMapAtlas->GetSourceMapUAV();
		output.bindIndex	= (uint)UAVBindIndex::VoxelMap_Emission;
		output.output		= _voxelEmissionMapAtlas->GetSourceMapUAV();

		outputs.push_back(output);
	}

	_clearVoxelMapCS->SetInputTextures(inputTextures);
	_clearVoxelMapCS->SetOutputs(outputs);
}

void Voxelization::Destroy()
{
	_voxelAlbedoMapAtlas->Destory();
	_voxelNormalMapAtlas->Destory();
	_voxelEmissionMapAtlas->Destory();

	_infoConstBuffer->Destory();

	_constBuffers.clear();
}

void Voxelization::ClearZeroVoxelMap(const Device::DirectX*& dx)
{
	_clearVoxelMapCS->Dispatch(dx->GetContext());
}

void Voxelization::Voxelize(const Device::DirectX*& dx,
							const MeshCamera*& camera, const RenderManager*& renderManager,
							const GlobalInfo& globalInfo,
							bool onlyStaticMesh)
{
	Math::Matrix camWorldMat;
	{
		const Transform* camTf = camera->GetOwner()->GetTransform();
		camTf->FetchWorldMatrix(camWorldMat);
	}

	if(onlyStaticMesh)
	{
		Matrix viewMat;
		CameraForm::GetViewMatrix(viewMat, camWorldMat);
		
		bool isDifferentViewMat = memcmp(&_prevStaticMeshVoxelizeViewMat, &viewMat, sizeof(Matrix)) != 0;
		if(isDifferentViewMat == false)
			return;

		_prevStaticMeshVoxelizeViewMat = viewMat;
	}

	ClearZeroVoxelMap(dx);
	Vector3 camWorldPos(camWorldMat._41, camWorldMat._42, camWorldMat._43);

	float cameraNear = camera->GetNear();
	float cameraFar  = camera->GetFar();
	ID3D11DeviceContext* context = dx->GetContext();

	context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	float blendFactor[1] = {0, };
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

	D3D11_VIEWPORT originViewport;
	uint originViewportNum = 0;
	context->RSGetViewports(&originViewportNum, &originViewport);

	float dimension = float(1 << globalInfo.voxelDimensionPow2);

	D3D11_VIEWPORT viewport;
	{
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= dimension;
		viewport.Height		= dimension;
	}
	context->RSSetViewports(1, &viewport);

	ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
	context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &samplerState);

	ID3D11UnorderedAccessView* uavs [] =
	{
		_voxelAlbedoMapAtlas->GetSourceMapUAV()->GetView(),
		_voxelNormalMapAtlas->GetSourceMapUAV()->GetView(),
		_voxelEmissionMapAtlas->GetSourceMapUAV()->GetView()
	};
	
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(uavs), uavs, nullptr);

	uint maxCascade = globalInfo.maxNumOfCascade;
	for(uint currentCascade=0; currentCascade<maxCascade; ++currentCascade)
	{
		// Compute & Update Const Buffers
		{
			// Compute Voxelize Bound
			float worldSize;
			Vector3 bbMin, bbMax, bbMid;
			ComputeBound(&bbMin, &bbMid, &bbMax, &worldSize, currentCascade, camWorldPos);

			InfoCBData currentVoxelizeInfo;
			currentVoxelizeInfo.currentCascade	= currentCascade;
			currentVoxelizeInfo.voxelizeSize	= worldSize;
			currentVoxelizeInfo.voxelSize		= worldSize / dimension;
			currentVoxelizeInfo.voxelizeMinPos	= bbMin;
			currentVoxelizeInfo.dummy1			= 0.0f;
			currentVoxelizeInfo.dummy2			= 0.0f;

			// Update View Proj ConstBuffer
			{
				Matrix orthoProjMat;
				Matrix::OrthoLH(orthoProjMat, worldSize, worldSize, cameraNear, cameraFar);

				auto LookAtView = [](
					Matrix& outViewMat,
					const Vector3& worldPos, const Vector3& targetPos, const Vector3& up
					)
				{
					Transform tf(nullptr);
					tf.UpdatePosition(worldPos);
					tf.LookAtWorld(targetPos, &up);

					Matrix worldMat;
					tf.FetchWorldMatrix(worldMat);

					CameraForm::GetViewMatrix(outViewMat, worldMat);
				};

				Matrix viewAxisX, viewAxisY, viewAxisZ;
				LookAtView(viewAxisZ, Vector3(bbMid.x, bbMid.y, bbMin.z), Vector3(bbMid.x, bbMid.y, bbMax.z), Vector3(0.0f, 1.0f, 0.0f)); //z
				LookAtView(viewAxisX, Vector3(bbMin.x, bbMid.y, bbMid.z), Vector3(bbMax.x, bbMid.y, bbMid.z), Vector3(0.0f, 1.0f, 0.0f)); //x
				LookAtView(viewAxisY, Vector3(bbMid.x, bbMin.y, bbMid.z), Vector3(bbMid.x, bbMax.y, bbMid.z), Vector3(0.0f, 0.0f,-1.0f)); //y

				currentVoxelizeInfo.viewProjX = viewAxisX * orthoProjMat;
				currentVoxelizeInfo.viewProjY = viewAxisY * orthoProjMat;
				currentVoxelizeInfo.viewProjZ = viewAxisZ * orthoProjMat;
			}

			_infoConstBuffer->UpdateSubResource(context, &currentVoxelizeInfo);
		}

		// Render Voxel
		{
			const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, RenderType::Voxelization, nullptr, nullptr, &_constBuffers);

			const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, RenderType::Voxelization, nullptr, nullptr, &_constBuffers);
		}
	}

	ID3D11UnorderedAccessView* nullUAVs[] = {nullptr, nullptr, nullptr};
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(nullUAVs), nullUAVs, nullptr);

	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &nullSampler);
	context->RSSetViewports(originViewportNum, &originViewport);
}

void Voxelization::ComputeVoxelVolumeProjMatrix(Math::Matrix& outMat, uint currentCascade, const Math::Vector3& camWorldPos) const
{
	memset(&outMat, 0, sizeof(Math::Matrix));

	float worldSize = 0.0f;
	Vector3 centerPos;
	ComputeBound(nullptr, &centerPos, nullptr, &worldSize, currentCascade, camWorldPos);

	ASSERT_COND_MSG(worldSize != 0.0f, "Error, Voxelize Size is zero");

	outMat._11 = 2.0f / worldSize;
	outMat._22 = 2.0f / worldSize;
	outMat._33 = 2.0f / worldSize;

	outMat._41 = 0.0f;
	outMat._42 = 0.0f;
	outMat._43 = 0.0f;
	outMat._44 = 1.0f;
}

void Voxelization::ComputeBound(
	Math::Vector3* outMin,
	Math::Vector3* outMid,
	Math::Vector3* outMax,
	float* outWorldSize,
	uint currentCascade,
	const Math::Vector3& camWorldPos) const
{
	float worldSize		= _initVoxelizationInfo.voxelizeSize * ( (float)( (currentCascade + 1) * (currentCascade + 1) ) );
	float halfWorldSize	= worldSize / 2.0f;

	Vector3 bbMin = camWorldPos - Vector3(halfWorldSize, halfWorldSize, halfWorldSize);
	Vector3 bbMax = bbMin + Vector3(worldSize, worldSize, worldSize);
	Vector3 bbMid = (bbMin + bbMax) / 2.0f;

	if(outMin)			(*outMin) = bbMin;
	if(outMid)			(*outMid) = bbMid;
	if(outMax)			(*outMax) = bbMax;
	if(outWorldSize)	(*outWorldSize) = worldSize;
}