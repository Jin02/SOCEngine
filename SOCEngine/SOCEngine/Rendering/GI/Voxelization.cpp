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
	: _viewProjAxisesConstBuffer(nullptr), _infoConstBuffer(nullptr),
	_maxNumOfCascade(0),
	_clearVoxelMapCS(nullptr), 
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
	SAFE_DELETE(_viewProjAxisesConstBuffer);
	SAFE_DELETE(_clearVoxelMapCS);
}

void Voxelization::Initialize(uint maxNumOfCascade, float minWorldSize, uint dimension)
{
	ASSERT_COND_MSG(maxNumOfCascade != 0, "Error, voxelization cascade num is zero.");
	_maxNumOfCascade = maxNumOfCascade;

	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

	const uint mipmapLevels = min((uint)Log2((float)dimension) + 1, 1);
	
	_voxelAlbedoMapAtlas = new AnisotropicVoxelMapAtlas;
	_voxelAlbedoMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels);

	_voxelNormalMapAtlas = new AnisotropicVoxelMapAtlas;
	_voxelNormalMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_UINT, mipmapLevels);

	_voxelEmissionMapAtlas = new AnisotropicVoxelMapAtlas;
	_voxelEmissionMapAtlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32_UINT, mipmapLevels);

	_infoConstBuffer = new ConstBuffer;
	_infoConstBuffer->Initialize(sizeof(Info));

	_viewProjAxisesConstBuffer = new ConstBuffer;
	_viewProjAxisesConstBuffer->Initialize(sizeof(ViewProjAxisesCBData));

	Info info;
	{
		info.dimension = dimension;
		info.voxelizeSize = minWorldSize;
	}
	UpdateInitVoxelizationInfo(info);

	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "ClearVoxelMap");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	ShaderManager* shaderMgr = ResourceManager::GetInstance()->GetShaderManager();
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
		inputTexture.bindIndex	= (uint)BindIndex::AlbedoUAV;
		inputTexture.texture	= _voxelAlbedoMapAtlas;
		inputTexture.bindIndex	= (uint)BindIndex::NormalUAV;
		inputTexture.texture	= _voxelNormalMapAtlas;
		inputTexture.bindIndex	= (uint)BindIndex::EmissionUAV;
		inputTexture.texture	= _voxelEmissionMapAtlas;

		inputTextures.push_back(inputTexture);
	}

	std::vector<ComputeShader::Output> outputs;
	{
		ComputeShader::Output output;
		output.bindIndex	= (uint)BindIndex::AlbedoUAV;
		output.output		= _voxelAlbedoMapAtlas->GetMipmapUAV(0);
		output.bindIndex	= (uint)BindIndex::NormalUAV;
		output.output		= _voxelNormalMapAtlas->GetMipmapUAV(0);
		output.bindIndex	= (uint)BindIndex::EmissionUAV;
		output.output		= _voxelEmissionMapAtlas->GetMipmapUAV(0);

		outputs.push_back(output);
	}

	_clearVoxelMapCS->SetInputTextures(inputTextures);
	_clearVoxelMapCS->SetOutputs(outputs);

	_inputConstBuffers.push_back(ShaderForm::InputConstBuffer((uint)BindIndex::InfoCB, _infoConstBuffer, false, true, false, true));
	_inputConstBuffers.push_back(ShaderForm::InputConstBuffer((uint)BindIndex::ViewProjAxisesCB, _viewProjAxisesConstBuffer, false, true, false, true));
}

void Voxelization::Destroy()
{
	_voxelAlbedoMapAtlas->Destory();
	_voxelNormalMapAtlas->Destory();
	_voxelEmissionMapAtlas->Destory();

	_infoConstBuffer->Destory();
	_viewProjAxisesConstBuffer->Destory();

	_inputConstBuffers.clear();
}

void Voxelization::ClearZeroVoxelMap(const Device::DirectX*& dx)
{
	_clearVoxelMapCS->Dispatch(dx->GetContext());
}

void Voxelization::Voxelize(const Device::DirectX*& dx, const MeshCamera*& camera, const RenderManager*& renderManager, bool onlyStaticMesh)
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

	ID3D11RasterizerState* originRSState = nullptr;
	context->RSGetState(&originRSState);

	ID3D11DepthStencilState* originDepthState = nullptr;
	uint originDepthStateRef = 0;
	context->OMGetDepthStencilState(&originDepthState, &originDepthStateRef);

	context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);
	float blendFactor[1] = {0, };
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);
	D3D11_VIEWPORT viewport;
	{
		viewport.TopLeftX	= 0.0f;
		viewport.TopLeftY	= 0.0f;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		viewport.Width		= (float)_initVoxelizationInfo.dimension;
		viewport.Height		= (float)_initVoxelizationInfo.dimension;
	}
	context->RSSetViewports(1, &viewport);

	ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
	context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &samplerState);

	D3D11_VIEWPORT originViewport;
	{
		uint vpNum = 1;
		context->RSGetViewports(&vpNum, &originViewport);
	}

	ID3D11UnorderedAccessView* uavs [] =
	{
		_voxelAlbedoMapAtlas->GetMipmapUAV(0)->GetView(),
		_voxelNormalMapAtlas->GetMipmapUAV(0)->GetView(),
		_voxelEmissionMapAtlas->GetMipmapUAV(0)->GetView()
	};
	
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(uavs), uavs, nullptr);

	for(uint currentCascade=0; currentCascade<_maxNumOfCascade; ++currentCascade)
	{
		// Compute & Update Const Buffers
		{
			// Compute Voxelize Bound
			float worldSize;
			Vector3 bbMin, bbMax, bbMid, worldMinPos;
			{
				ComputeBound(&bbMin, &bbMid, &bbMax, &worldSize, &worldMinPos, currentCascade, camWorldPos);
			}

			// Update Voxelize Info CB
			{
				InfoCBData currentVoxelizeInfo;
				{
					currentVoxelizeInfo.currentCascade	= currentCascade;
					currentVoxelizeInfo.dimension		= _initVoxelizationInfo.dimension;

					currentVoxelizeInfo.voxelizeSize	= worldSize;
					currentVoxelizeInfo.voxelSize		= worldSize / (float)currentVoxelizeInfo.dimension;

					currentVoxelizeInfo.voxelizeMinPos	= worldMinPos;
					currentVoxelizeInfo.dummy = 0;
				}
				
				_infoConstBuffer->UpdateSubResource(context, &currentVoxelizeInfo);
			}

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

				ViewProjAxisesCBData viewProjAxises;
				viewProjAxises.viewProjX = viewAxisX * orthoProjMat;
				viewProjAxises.viewProjY = viewAxisY * orthoProjMat;
				viewProjAxises.viewProjZ = viewAxisZ * orthoProjMat;

				_viewProjAxisesConstBuffer->UpdateSubResource(context, &viewProjAxises);
			}
		}

		// Render Voxel
		{
			const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, MeshCamera::RenderType::Voxelization, nullptr, nullptr, &_inputConstBuffers);

			const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, MeshCamera::RenderType::Voxelization, nullptr, nullptr, &_inputConstBuffers);
		}
	}

	ID3D11UnorderedAccessView* nullUAVs[] = {nullptr, nullptr, nullptr};
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(nullUAVs), nullUAVs, nullptr);

	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers((uint)SamplerStateBindIndex::DefaultSamplerState, 1, &nullSampler);
	context->RSSetState(originRSState);
	context->RSSetViewports(1, &originViewport);
	context->OMSetDepthStencilState(originDepthState, originDepthStateRef);
}

void Voxelization::UpdateInitVoxelizationInfo(const Info& info)
{
	InfoCBData& initInfo = _initVoxelizationInfo;

	initInfo.currentCascade	= 0;
	initInfo.voxelizeSize	= info.voxelizeSize;
	initInfo.dimension		= info.dimension;
	initInfo.voxelSize		= info.voxelizeSize / (float)info.dimension;
}

void Voxelization::ComputeVoxelVolumeProjMatrix(Math::Matrix& outMat, uint currentCascade, const Math::Vector3& camWorldPos) const
{
	memset(&outMat, 0, sizeof(Math::Matrix));

	float worldSize = 0.0f;
	Vector3 centerPos;
	ComputeBound(nullptr, &centerPos, nullptr, &worldSize, nullptr, currentCascade, camWorldPos);

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
	Math::Vector3* outVoxelizeMinPos,
	uint currentCascade,
	const Math::Vector3& camWorldPos) const
{
	float worldSize		= _initVoxelizationInfo.voxelizeSize * ( (float)( (currentCascade + 1) * (currentCascade + 1) ) );
	float offset		= (worldSize / (float)(currentCascade + 1)) / 2.0f;
	Vector3 worldMinPos	= camWorldPos - Vector3(offset, offset, offset);

	float halfWorldSize	= worldSize / 2.0f;
	float cascadeScale	= (float)(currentCascade + 1);

	Vector3 bbMin = worldMinPos * Vector3(cascadeScale, cascadeScale, cascadeScale);
	Vector3 bbMax = bbMin + Vector3(worldSize, worldSize, worldSize);
	Vector3 bbMid = (bbMin + bbMax) / 2.0f;

	if(outMin)			(*outMin) = bbMin;
	if(outMid)			(*outMid) = bbMid;
	if(outMax)			(*outMax) = bbMax;
	if(outWorldSize)	(*outWorldSize) = worldSize;
	
	if(outVoxelizeMinPos)
		(*outVoxelizeMinPos) = worldMinPos;
}