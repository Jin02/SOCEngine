#include "TBRShaderIndexSlotInfo.h"
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
using namespace GPGPU::DirectCompute;

Voxelization::Voxelization()
	: _viewProjAxisesConstBuffer(nullptr), _infoConstBuffer(nullptr),
	_maxNumOfCascade(0), _changedInitVoxelizationInfo(false),
	_clearVoxelMapCS(nullptr)
{
}

Voxelization::~Voxelization()
{
	Destroy();

	for(auto& iter : _voxelMapAtlas)	SAFE_DELETE(iter);
	_voxelMapAtlas.clear();

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
	
	for(uint i=0; i<maxNumOfCascade; ++i)
	{
		AnisotropicVoxelMapAtlas* atlas = new AnisotropicVoxelMapAtlas;
		atlas->Initialize(dimension, maxNumOfCascade, DXGI_FORMAT_R32G32B32_UINT, mipmapLevels, (uint)BindIndex::InOutVoxelMap);

		_voxelMapAtlas.push_back(atlas);
	}

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
		inputTexture.bindIndex	= (uint)BindIndex::InOutVoxelMap;
		inputTexture.texture	= nullptr;

		inputTextures.push_back(inputTexture);
	}

	std::vector<ComputeShader::Output> outputs;
	{
		ComputeShader::Output output;
		output.bindIndex	= (uint)BindIndex::InOutVoxelMap;
		output.output		= nullptr;

		outputs.push_back(output);
	}

	_clearVoxelMapCS->SetInputTextures(inputTextures);
	_clearVoxelMapCS->SetOutputs(outputs);
}

void Voxelization::Destroy()
{
	for(auto& iter : _voxelMapAtlas)	iter->Destroy();

	_infoConstBuffer->Destory();
	_viewProjAxisesConstBuffer->Destory();
}

void Voxelization::Clear(const Device::DirectX*& dx)
{

}

void Voxelization::Voxelize(const Device::DirectX*& dx, const MeshCamera*& camera, const RenderManager*& renderManager)
{
	Math::Matrix camWorldMat;
	{
		const Transform* camTf = camera->GetOwner()->GetTransform();
		camTf->FetchWorldMatrix(camWorldMat);
	}

	//Check Duplicated Work.
	{
		Matrix viewMat;
		CameraForm::GetViewMatrix(viewMat, camWorldMat);
		
		bool isDifferentViewMat = memcmp(&_prevViewMat, &viewMat, sizeof(Matrix)) != 0;
		if((isDifferentViewMat || _changedInitVoxelizationInfo) == false)
			return;

		_prevViewMat = viewMat;
		_changedInitVoxelizationInfo = false;
	}

	Clear(dx);
	Vector3 camWorldPos(camWorldMat._41, camWorldMat._42, camWorldMat._43);

	float cameraNear = camera->GetNear();
	float cameraFar  = camera->GetFar();
	ID3D11DeviceContext* context = dx->GetContext();

	D3D11_VIEWPORT originViewport;
	{
		uint vpNum = 1;
		context->RSGetViewports(&vpNum, &originViewport);
	}
	ID3D11RasterizerState* originRSState = nullptr;
	context->RSGetState(&originRSState);

	ID3D11DepthStencilState* originDepthState = nullptr;
	uint originDepthStateRef = 0;
	context->OMGetDepthStencilState(&originDepthState, &originDepthStateRef);

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
	context->RSSetState(dx->GetRasterizerStateCWDisableCulling());
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);
	float blendFactor[1] = {0, };
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

	ID3D11SamplerState* samplerState = dx->GetSamplerStateAnisotropic();
	context->PSSetSamplers((uint)TBDR::InputSamplerStateBindSlotIndex::DefaultSamplerState, 1, &samplerState);

	// 어짜피, 모든 복셀맵이 같은 인덱스를 공유하고 있으므로 하나만 해도된다.
	_voxelMapAtlas[0]->BindUAVsToPixelShader(dx);

	for(uint currentCascade=0; currentCascade<_maxNumOfCascade; ++currentCascade)
	{
		// Compute & Update Const Buffers
		{
			InfoCBData currentVoxelizeInfo;
			{
				currentVoxelizeInfo.currentCascade	= currentCascade;
				currentVoxelizeInfo.dimension		= _initVoxelizationInfo.dimension;

				uint uscale = currentCascade + 1;
				float scale = (float)(uscale * uscale);
				currentVoxelizeInfo.voxelizeSize	= _initVoxelizationInfo.voxelizeSize * scale;
				currentVoxelizeInfo.voxelSize		= currentVoxelizeInfo.voxelizeSize / (float)currentVoxelizeInfo.dimension;

				currentVoxelizeInfo.dummy = 0;
			}

			float worldSize = currentVoxelizeInfo.voxelizeSize;

			Matrix orthoProjMat;
			Matrix::OrthoLH(orthoProjMat, worldSize, worldSize, cameraNear, cameraFar);

			float halfWorldSize = worldSize / 2.0f;
			float cascadeScale = (float)currentCascade + 1.0f;

			float offset = (worldSize / (float)(currentCascade + 1)) / 2.0f;
			Vector3 worldMinPos = camWorldPos - Vector3(offset, offset, offset);
			currentVoxelizeInfo.voxelizeMinPos = worldMinPos;

			_infoConstBuffer->UpdateSubResource(context, &currentVoxelizeInfo);
			
			Vector3 bbMin = worldMinPos * Vector3(cascadeScale, cascadeScale, cascadeScale);
			Vector3 bbMax = bbMin + Vector3(worldSize, worldSize, worldSize);
			Vector3 bbMid = (bbMin + bbMax) / 2.0f;

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

		// Render Voxel
		{
			const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, MeshCamera::RenderType::Voxelization, nullptr);

			const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
			MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, MeshCamera::RenderType::Voxelization, nullptr);
		}
	}

	// 어짜피, 모든 복셀맵이 같은 인덱스를 공유하고 있으므로 하나만 해도된다.
	_voxelMapAtlas[0]->UnbindUAVs(dx);

	ID3D11SamplerState* nullSampler = nullptr;
	context->PSSetSamplers((uint)TBDR::InputSamplerStateBindSlotIndex::DefaultSamplerState, 1, &nullSampler);
	context->RSSetViewports(1, &originViewport);
	context->RSSetState(originRSState);
	context->OMSetDepthStencilState(originDepthState, originDepthStateRef);
}

void Voxelization::UpdateInitVoxelizationInfo(const Info& info)
{
	InfoCBData& initInfo = _initVoxelizationInfo;

	initInfo.currentCascade	= 0;
	initInfo.voxelizeSize	= info.voxelizeSize;
	initInfo.dimension		= info.dimension;
	initInfo.voxelSize		= info.voxelizeSize / (float)info.dimension;

	_changedInitVoxelizationInfo = true;
}