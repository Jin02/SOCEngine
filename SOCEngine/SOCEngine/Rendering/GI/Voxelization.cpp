#include "BindIndexInfo.h"
#include "Voxelization.h"
#include "Object.h"
#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

#include "MeshCamera.h"

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
:	_clearVoxelMapCS(nullptr),
	_voxelAlbedoMapAtlas(nullptr), _voxelNormalMapAtlas(nullptr), _voxelEmissionMapAtlas(nullptr), _globalInfoCB(nullptr)
{
}

Voxelization::~Voxelization()
{
	Destroy();

	SAFE_DELETE(_voxelAlbedoMapAtlas);
	SAFE_DELETE(_voxelNormalMapAtlas);
	SAFE_DELETE(_voxelEmissionMapAtlas);

	for(auto iter = _constBuffers.begin(); iter != _constBuffers.end(); ++iter)
		SAFE_DELETE(*iter);
	_constBuffers.clear();

	SAFE_DELETE(_clearVoxelMapCS);
}

void Voxelization::Initialize(const GlobalInfo& globalInfo, const ConstBuffer* globalInfoCB)
{
	uint maxNumOfCascade = globalInfo.maxCascadeWithVoxelDimensionPow2 >> 16;
	ASSERT_COND_MSG(maxNumOfCascade != 0, "Error, voxelization cascade num is zero.");

	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

#if defined(USE_ANISOTROPIC_VOXELIZATION)
	bool isAnisotropic = true;
#else
	bool isAnisotropic = false;
#endif

	uint dimension = 1 << (globalInfo.maxCascadeWithVoxelDimensionPow2 & 0xffff);
	
	_voxelAlbedoMapAtlas = new VoxelMap;
	_voxelAlbedoMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, 1, isAnisotropic);

	_voxelNormalMapAtlas = new VoxelMap;
#if defined(USE_ANISOTROPIC_VOXELIZATION)
	_voxelNormalMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_UINT, 1, isAnisotropic);
#else
	_voxelNormalMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, 1, isAnisotropic);
#endif
	_voxelEmissionMapAtlas = new VoxelMap;
	_voxelEmissionMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, 1, isAnisotropic);

	// Setting Const Buffers
	{
		for(uint i=0; i<maxNumOfCascade; ++i)
		{
			ConstBuffer* infoConstBuffer = new ConstBuffer;
			infoConstBuffer->Initialize(sizeof(InfoCBData));

			_constBuffers.push_back(infoConstBuffer);
		}

//		_renderInputCBContainer.push_back( ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::Voxelization_InfoCB), nullptr, false, true, false, true) );
	}

	InitializeClearVoxelMap(dimension, maxNumOfCascade);
	_globalInfoCB = globalInfoCB;
}

void Voxelization::InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade)
{
	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "ClearVoxelMaps");

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
#if defined(USE_ANISOTROPIC_VOXELIZATION)
		threadGroup.x = ComputeThreadGroupSideLength(dimension) * (uint)VoxelMap::Direction::Num;
#else
		threadGroup.x = ComputeThreadGroupSideLength(dimension);
#endif
		threadGroup.y = ComputeThreadGroupSideLength(dimension * maxNumOfCascade);
		threadGroup.z = ComputeThreadGroupSideLength(dimension);
	}

	_clearVoxelMapCS = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_clearVoxelMapCS->Initialize(), "Error, Can't Init ClearVoxelMapCS");

	std::vector<ShaderForm::InputUnorderedAccessView> uavs;
	{
		ShaderForm::InputUnorderedAccessView uav;
		uav.bindIndex	= (uint)UAVBindIndex::VoxelMap_Albedo;
		uav.uav			= _voxelAlbedoMapAtlas->GetSourceMapUAV();
		uavs.push_back(uav);

		uav.bindIndex	= (uint)UAVBindIndex::VoxelMap_Normal;
		uav.uav			= _voxelNormalMapAtlas->GetSourceMapUAV();
		uavs.push_back(uav);

		uav.bindIndex	= (uint)UAVBindIndex::VoxelMap_Emission;
		uav.uav			= _voxelEmissionMapAtlas->GetSourceMapUAV();
		uavs.push_back(uav);
	}

	_clearVoxelMapCS->SetUAVs(uavs);
}

void Voxelization::Destroy()
{
	_voxelAlbedoMapAtlas->Destory();
	_voxelNormalMapAtlas->Destory();
	_voxelEmissionMapAtlas->Destory();

	for(auto iter = _constBuffers.begin(); iter != _constBuffers.end(); ++iter)
		(*iter)->Destory();

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

	ID3D11DeviceContext* context = dx->GetContext();

	context->RSSetState(dx->GetRasterizerStateCWDisableCullingWithClip());
	context->OMSetDepthStencilState(dx->GetDepthStateLessEqual(), 0x00);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

	float dimension = float(1 << (globalInfo.maxCascadeWithVoxelDimensionPow2 & 0xffff));

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

	float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	context->ClearRenderTargetView(_voxelAlbedoMapAtlas->GetRenderTargetView(), clearColor);
	context->ClearRenderTargetView(_voxelNormalMapAtlas->GetRenderTargetView(), clearColor);
	context->ClearRenderTargetView(_voxelEmissionMapAtlas->GetRenderTargetView(), clearColor);
	
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(uavs), uavs, nullptr);

	ID3D11Buffer* globalInfoBuffer = _globalInfoCB->GetBuffer();
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::GlobalIIllumination_InfoCB), 1, &globalInfoBuffer);

	uint maxCascade = globalInfo.maxCascadeWithVoxelDimensionPow2 >> 16;

	for(uint currentCascade=0; currentCascade<maxCascade; ++currentCascade)
	{
		UpdateConstBuffer(dx, currentCascade, camWorldPos, globalInfo, dimension);

		// Render Voxel
		{
			ID3D11Buffer* buf = _constBuffers[currentCascade]->GetBuffer();
			context->GSSetConstantBuffers(uint(ConstBufferBindIndex::Voxelization_InfoCB), 1, &buf);
			context->PSSetConstantBuffers(uint(ConstBufferBindIndex::Voxelization_InfoCB), 1, &buf);

			const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, RenderType::Voxelization, nullptr);

			const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, RenderType::Voxelization, nullptr);
		}
	}

	ID3D11Buffer* nullBuff = nullptr;
	context->GSSetConstantBuffers(uint(ConstBufferBindIndex::Voxelization_InfoCB),			1, &nullBuff);
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::Voxelization_InfoCB),			1, &nullBuff);
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::GlobalIIllumination_InfoCB),	1, &nullBuff);

	ID3D11UnorderedAccessView* nullUAVs[] = {nullptr, nullptr, nullptr};
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(nullUAVs), nullUAVs, nullptr);

	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &nullSampler);

	context->RSSetState(nullptr);
}

void Voxelization::UpdateConstBuffer(const Device::DirectX*& dx, uint currentCascade,
									 const Vector3& camWorldPos, const GlobalInfo& globalInfo, float dimension)
{
	// Compute Voxelize Bound
	float worldSize;
	Vector3 bbMin, bbMax, bbMid;
	ComputeBound(&bbMin, &bbMid, &bbMax, &worldSize, currentCascade, camWorldPos, globalInfo.initWorldSize);

	InfoCBData currentVoxelizeInfo;
	currentVoxelizeInfo.currentCascade	= currentCascade;
	currentVoxelizeInfo.voxelizeMinPos	= bbMin;

	// Update View Proj ConstBuffer
	{
		Matrix voxelView;
		ComputeVoxelViewMatrix(voxelView, currentCascade, camWorldPos, globalInfo.initWorldSize);

		Matrix projMat;
		Matrix::OrthoLH(projMat, 2.0f, 2.0f, 1.0f, -1.0f);

		currentVoxelizeInfo.voxelView		= voxelView;
		currentVoxelizeInfo.voxelViewProj	= voxelView * projMat;
	}

	_constBuffers[currentCascade]->UpdateSubResource(dx->GetContext(), &currentVoxelizeInfo);
}

void Voxelization::ComputeVoxelViewMatrix(	Math::Matrix& outMat, uint currentCascade,
											const Math::Vector3& camWorldPos, float initVoxelizeSize)
{
	memset(&outMat, 0, sizeof(Math::Matrix));

	float worldSize = 0.0f;
	Vector3 centerPos;
	ComputeBound(nullptr, &centerPos, nullptr, &worldSize, currentCascade, camWorldPos, initVoxelizeSize);

	ASSERT_COND_MSG(worldSize != 0.0f, "Error, Voxelize Size is zero");

	outMat._11 = 2.0f / worldSize;
	outMat._22 = 2.0f / worldSize;
	outMat._33 = 2.0f / worldSize;

	outMat._41 = camWorldPos.x;
	outMat._42 = camWorldPos.y;
	outMat._43 = camWorldPos.z;
	outMat._44 = 1.0f;
}

void Voxelization::ComputeBound(
	Vector3* outMin, Vector3* outMid, Vector3* outMax, float* outWorldSize,
	uint currentCascade, const Vector3& camWorldPos, float initVoxelizeSize)
{
	float worldSize		= initVoxelizeSize * ( (float)( (currentCascade + 1) * (currentCascade + 1) ) );
	float halfWorldSize	= worldSize / 2.0f;

	Vector3 bbMin = camWorldPos - Vector3(halfWorldSize, halfWorldSize, halfWorldSize);
	Vector3 bbMax = bbMin + Vector3(worldSize, worldSize, worldSize);
	Vector3 bbMid = (bbMin + bbMax) / 2.0f;

	if(outMin)			(*outMin) = bbMin;
	if(outMid)			(*outMid) = bbMid;
	if(outMax)			(*outMax) = bbMax;
	if(outWorldSize)	(*outWorldSize) = worldSize;
}