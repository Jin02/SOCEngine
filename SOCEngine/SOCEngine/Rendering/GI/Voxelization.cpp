#include "BindIndexInfo.h"
#include "Voxelization.h"
#include "Object.h"
#include "ResourceManager.h"
#include "EngineShaderFactory.hpp"

#include "MeshCamera.h"

#include "Scene.h"

using namespace Resource;
using namespace Core;
using namespace Math;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Camera;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::View;
using namespace GPGPU::DirectCompute;

Voxelization::Voxelization()
:	_clearVoxelMapCS(nullptr), _voxelAlbedoRawBuffer(nullptr), _voxelNormalRawBuffer(nullptr), _voxelEmissionRawBuffer(nullptr), _infoCB(nullptr)
{
}

Voxelization::~Voxelization()
{
	Destroy();

	SAFE_DELETE(_voxelAlbedoRawBuffer);
	SAFE_DELETE(_voxelNormalRawBuffer);
	SAFE_DELETE(_voxelEmissionRawBuffer);
	SAFE_DELETE(_infoCB);

	SAFE_DELETE(_clearVoxelMapCS);
}

void Voxelization::Initialize(uint dimension)
{
	auto Log2 = [](float v) -> float
	{
		return log(v) / log(2.0f);
	};

	uint count = dimension * dimension * dimension;
	
	_voxelAlbedoRawBuffer	= new RawBuffer;
	_voxelAlbedoRawBuffer->Initialize(4, count, RawBuffer::Flag::ALL_VIEW);

	_voxelEmissionRawBuffer	= new RawBuffer;
	_voxelEmissionRawBuffer->Initialize(4, count, RawBuffer::Flag::ALL_VIEW);

	_voxelNormalRawBuffer = new RawBuffer;
	_voxelNormalRawBuffer->Initialize(4, count, RawBuffer::Flag::ALL_VIEW);

	if(_infoCB == nullptr) _infoCB = new ConstBuffer;
	_infoCB->Initialize(sizeof(InfoCBData));

	InitializeClearVoxelMap(dimension);
}

void Voxelization::InitializeClearVoxelMap(uint dimension)
{
	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "ClearVoxelRawMap");

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

		uint length = ComputeThreadGroupSideLength(dimension);
		threadGroup.x = threadGroup.y = threadGroup.z = length;
	}

	_clearVoxelMapCS = new ComputeShader(threadGroup, blob);
	ASSERT_COND_MSG(_clearVoxelMapCS->Initialize(), "Error, Can't Init ClearVoxelMapCS");
}

void Voxelization::Destroy()
{
	_voxelAlbedoRawBuffer->Destroy();
	_voxelNormalRawBuffer->Destroy();
	_voxelEmissionRawBuffer->Destroy();

	_infoCB->Destroy();
}

void Voxelization::ClearZeroVoxelMap(const Device::DirectX*& dx, const ConstBuffer* vxgiStaticInfoCB)
{
	ID3D11DeviceContext* context = dx->GetContext();

	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMap_Albedo,				_voxelAlbedoRawBuffer->GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMap_Emission,			_voxelEmissionRawBuffer->GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMap_Normal,				_voxelNormalRawBuffer->GetUnorderedAccessView());

	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::VXGIStaticInfoCB,		vxgiStaticInfoCB);

	_clearVoxelMapCS->Dispatch(context);

	ComputeShader::BindConstBuffer(context,			ConstBufferBindIndex::VXGIStaticInfoCB,		nullptr);

	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMap_Albedo,				nullptr);
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMap_Emission,			nullptr);
	ComputeShader::BindUnorderedAccessView(context, UAVBindIndex::VoxelMap_Normal,				nullptr);
}

void Voxelization::Voxelize(const Device::DirectX*& dx,
							const Vector3& startMinWorldPos, const Core::Scene* scene,
							float dimension, float voxelSize, const VoxelMap* injectionColorMap,
							const ConstBuffer* vxgiStaticInfoCB, const ConstBuffer* vxgiDynamicInfoCB)
{
	ClearZeroVoxelMap(dx, vxgiStaticInfoCB);
	ID3D11DeviceContext* context = dx->GetContext();

	context->RSSetState(dx->GetRasterizerStateCWDisableCullingWithClip());
	context->OMSetDepthStencilState(dx->GetDepthStateLessEqual(), 0x00);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	context->OMSetBlendState(dx->GetBlendStateOpaque(), blendFactor, 0xffffffff);

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

	ID3D11UnorderedAccessView* uavs [] =
	{
		_voxelAlbedoRawBuffer->GetUnorderedAccessView()->GetView(),
		_voxelNormalRawBuffer->GetUnorderedAccessView()->GetView(),
		_voxelEmissionRawBuffer->GetUnorderedAccessView()->GetView(),
		injectionColorMap->GetSourceMapUAV()->GetView()
	};
	
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(uavs), uavs, nullptr);

	const LightManager*		lightMgr	= scene->GetLightManager();
	const ShadowRenderer*	shadowMgr	= scene->GetShadowManager();

	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightDirXY,					lightMgr->GetDirectionalLightDirXYSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightColor,					lightMgr->GetDirectionalLightColorSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightShadowParam,				shadowMgr->GetDirectionalLightShadowParamSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightOptionalParamIndex,		lightMgr->GetDirectionalLightOptionalParamIndexSRBuffer());
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightShadowViewProjMatrix,		shadowMgr->GetDirectionalLightShadowViewProjSRBuffer());
	
	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::VXGIStaticInfoCB,						vxgiStaticInfoCB);
	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::VXGIDynamicInfoCB,					vxgiDynamicInfoCB);

	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::ShadowGlobalParam,					shadowMgr->GetShadowGlobalParamConstBuffer());

	PixelShader::BindTexture(context,					TextureBindIndex::DirectionalLightShadowMapAtlas,			shadowMgr->GetDirectionalLightShadowMapAtlas());

	PixelShader::BindSamplerState(context,				SamplerStateBindIndex::ShadowComprisonSamplerState,			dx->GetShadowGreaterEqualSamplerComparisonState());	
	PixelShader::BindSamplerState(context,				SamplerStateBindIndex::DefaultSamplerState,					dx->GetSamplerStateAnisotropic());	
	PixelShader::BindSamplerState(context,				SamplerStateBindIndex::VSMShadowSamplerState,				dx->GetShadowSamplerState());	

	const RenderManager* renderManager = scene->GetRenderManager();
	UpdateConstBuffer(dx, startMinWorldPos, voxelSize, dimension);

	GeometryShader::BindConstBuffer(context,			ConstBufferBindIndex::VoxelizationInfoCB,					_infoCB);
	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::VoxelizationInfoCB,					_infoCB);

	// Render Voxel
	{
		const auto& opaqueMeshes = renderManager->GetOpaqueMeshes();
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, opaqueMeshes, RenderType::Voxelization, nullptr, nullptr);

		const auto& alphaTestMeshes = renderManager->GetAlphaTestMeshes();
		MeshCamera::RenderMeshesUsingSortedMeshVectorByVB(dx, renderManager, alphaTestMeshes, RenderType::Voxelization, nullptr, nullptr);
	}
	

	GeometryShader::BindConstBuffer(context, 			ConstBufferBindIndex::VoxelizationInfoCB,					nullptr);
	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::VoxelizationInfoCB,					nullptr);

	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightDirXY,					nullptr);
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightColor,					nullptr);
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightShadowParam,				nullptr);
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightOptionalParamIndex,		nullptr);
	PixelShader::BindShaderResourceBuffer(context,		TextureBindIndex::DirectionalLightShadowViewProjMatrix,		nullptr);

	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::VXGIStaticInfoCB,						nullptr);
	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::VXGIDynamicInfoCB,					nullptr);
	PixelShader::BindConstBuffer(context,				ConstBufferBindIndex::ShadowGlobalParam,					nullptr);

	PixelShader::BindTexture(context,					TextureBindIndex::DirectionalLightShadowMapAtlas,			nullptr);

	PixelShader::BindSamplerState(context,				SamplerStateBindIndex::ShadowComprisonSamplerState,			nullptr);
	PixelShader::BindSamplerState(context,				SamplerStateBindIndex::DefaultSamplerState,					nullptr);	
	PixelShader::BindSamplerState(context,				SamplerStateBindIndex::VSMShadowSamplerState,				nullptr);	

	ID3D11UnorderedAccessView* nullUAVs[] = {nullptr, nullptr, nullptr, nullptr};
	context->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, ARRAYSIZE(nullUAVs), nullUAVs, nullptr);

	context->RSSetState(nullptr);
}

void Voxelization::UpdateConstBuffer(const Device::DirectX*& dx, const Vector3& startCenterWorldPos, float voxelSize, float dimension)
{
	// Compute Voxelize Bound
	float worldSize;
	Vector3 bbMin, bbMax, bbMid;
	ComputeBound(&bbMin, &bbMid, &bbMax, &worldSize, startCenterWorldPos, voxelSize * dimension);

	InfoCBData currentVoxelizeInfo;
	currentVoxelizeInfo.voxelizeMinPos	= Vector4(bbMin.x, bbMin.y, bbMin.z, 1.0f);

	// Update View Proj ConstBuffer
	{
		Matrix orthoProjMat;
		Matrix::OrthoLH(orthoProjMat, worldSize, worldSize, 0.0f, worldSize);

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

		Vector3 center = bbMid;

		Matrix viewAxisX, viewAxisY, viewAxisZ;

		float halfWorldSize = worldSize / 2.0f;
		LookAtView(viewAxisX, center + Vector3(halfWorldSize, 0.0f, 0.0f), center, Vector3(0.0f, 1.0f, 0.0f)); //x
		LookAtView(viewAxisY, center + Vector3(0.0f, halfWorldSize, 0.0f), center, Vector3(0.0f, 0.0f,-1.0f)); //y
		LookAtView(viewAxisZ, center + Vector3(0.0f, 0.0f, halfWorldSize), center, Vector3(0.0f, 1.0f, 0.0f)); //z

		currentVoxelizeInfo.viewProjX = viewAxisX * orthoProjMat;
		currentVoxelizeInfo.viewProjY = viewAxisY * orthoProjMat;
		currentVoxelizeInfo.viewProjZ = viewAxisZ * orthoProjMat;

#ifdef USE_BLOATING_IN_VOXELIZATION_PASS 
		Matrix::Inverse(currentVoxelizeInfo.viewProjX_inv, currentVoxelizeInfo.viewProjX_inv);
		Matrix::Inverse(currentVoxelizeInfo.viewProjY_inv, currentVoxelizeInfo.viewProjY_inv);
		Matrix::Inverse(currentVoxelizeInfo.viewProjZ_inv, currentVoxelizeInfo.viewProjZ_inv);

		Matrix::Transpose(currentVoxelizeInfo.viewProjX_inv, currentVoxelizeInfo.viewProjX_inv);
		Matrix::Transpose(currentVoxelizeInfo.viewProjY_inv, currentVoxelizeInfo.viewProjY_inv);
		Matrix::Transpose(currentVoxelizeInfo.viewProjZ_inv, currentVoxelizeInfo.viewProjZ_inv);
#endif
		Matrix::Transpose(currentVoxelizeInfo.viewProjX, currentVoxelizeInfo.viewProjX);
		Matrix::Transpose(currentVoxelizeInfo.viewProjY, currentVoxelizeInfo.viewProjY);
		Matrix::Transpose(currentVoxelizeInfo.viewProjZ, currentVoxelizeInfo.viewProjZ);	
	}

	_infoCB->UpdateSubResource(dx->GetContext(), &currentVoxelizeInfo);
}

void Voxelization::ComputeBound(
	Vector3* outMin, Vector3* outMid, Vector3* outMax, float* outWorldSize,
	const Vector3& startCenterWorldPos, float voxelizationSize)
{
	float halfWorldSize	= voxelizationSize / 2.0f;

	Vector3 bbMin = startCenterWorldPos - Vector3(halfWorldSize, halfWorldSize, halfWorldSize);
	Vector3 bbMax = bbMin + Vector3(voxelizationSize, voxelizationSize, voxelizationSize);
	Vector3 bbMid = (bbMin + bbMax) / 2.0f;

	if(outMin)			(*outMin) = bbMin;
	if(outMid)			(*outMid) = bbMid;
	if(outMax)			(*outMax) = bbMax;
	if(outWorldSize)	(*outWorldSize) = voxelizationSize;
}
