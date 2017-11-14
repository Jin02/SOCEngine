#include "BindIndexInfo.h"
#include "Voxelization.h"
#include "ShaderFactory.hpp"
#include "MeshUtility.h"
#include "DirectionalLight.h"

using namespace Core;
using namespace Math;
using namespace Device;
using namespace Rendering;
using namespace Intersection;
using namespace Rendering::Buffer;
using namespace Rendering::GI;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Renderer;
using namespace Rendering::View;
using namespace Rendering::Factory;
using namespace Rendering::Light;
using namespace Rendering::Shadow;
using namespace Rendering::RenderState;
using namespace Rendering::Geometry;
using namespace Rendering::Material;

void Voxelization::Initialize(DirectX& dx, ShaderManager& shaderMgr, uint dimension, float voxelSize)
{
	_dimension = static_cast<float>(dimension);
	_worldSize = _dimension * voxelSize;

	uint count = dimension * dimension * dimension;
	
	_voxelAlbedoRawBuffer.Initialize(dx,	4, count, GPURawBuffer::Flag::ALL_VIEW);
	_voxelEmissionRawBuffer.Initialize(dx,	4, count, GPURawBuffer::Flag::ALL_VIEW);
	_voxelNormalRawBuffer.Initialize(dx,	4, count, GPURawBuffer::Flag::ALL_VIEW);

	_infoCB.Initialize(dx);

	// Init Clear VoxelMap
	ShaderFactory factory(&shaderMgr);
	_clearVoxelRawMapCS = *factory.LoadComputeShader(dx, "ClearVoxelRawMap", "ClearVoxelMapCS", nullptr, "@ClearVoxelRawMap");
}

void Voxelization::ClearVoxelMap(DirectX& dx, const ExplicitConstBuffer<VXGIStaticInfo>& vxgiStaticInfoCB)
{
	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex::VoxelMap_Albedo,	_voxelAlbedoRawBuffer.GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex::VoxelMap_Emission,	_voxelEmissionRawBuffer.GetUnorderedAccessView());
	ComputeShader::BindUnorderedAccessView(dx, UAVBindIndex::VoxelMap_Normal,	_voxelNormalRawBuffer.GetUnorderedAccessView());

	ComputeShader::BindConstBuffer(dx, ConstBufferBindIndex::VXGIStaticInfoCB, vxgiStaticInfoCB);


	auto ComputeThreadGroupSideLength = [](uint sideLength)
	{
		return static_cast<uint>(static_cast<float>(sideLength + 8 - 1) / 8.0f);
	};

	uint length = ComputeThreadGroupSideLength(static_cast<uint>(_dimension));
	_clearVoxelRawMapCS.Dispatch(dx, ComputeShader::ThreadGroup(length, length, length));

	ComputeShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIStaticInfoCB);

	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::VoxelMap_Albedo);
	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::VoxelMap_Emission);
	ComputeShader::UnBindUnorderedAccessView(dx, UAVBindIndex::VoxelMap_Normal);
}

void Voxelization::Voxelize(DirectX& dx, VoxelMap& outDLInjectVoxelMap, const Voxelization::Param&& param)
{
	ClearVoxelMap(dx, param.infoCB.staticInfoCB);

	UnorderedAccessView* uavs[] =
	{
		&_voxelAlbedoRawBuffer.GetUnorderedAccessView(),
		&_voxelNormalRawBuffer.GetUnorderedAccessView(),
		&_voxelEmissionRawBuffer.GetUnorderedAccessView(),
		&outDLInjectVoxelMap.GetSourceMapUAV()
	};

	dx.SetUAVsWithoutRenderTarget(0, ARRAYSIZE(uavs), uavs);
	dx.SetRasterizerState(RasterizerState::CWDisableCullingWithClip);
	dx.SetDepthStencilState(DepthState::LessEqual, 0);
	dx.SetBlendState(BlendState::Opaque);
	dx.SetViewport(Rect<float>(0.0f, 0.0f, _dimension, _dimension));
	dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	const auto& dlBuffer = param.lightMgr.GetBuffer<DirectionalLight>();
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex::DirectionalLightDirXY,					dlBuffer.GetTransformSRBuffer().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex::DirectionalLightColor,					dlBuffer.GetColorSRBuffer().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,		dlBuffer.GetOptionalParamIndexSRBuffer().GetShaderResourceView());

	const auto& dlsBuffer = param.shadowSystem.manager.GetBuffer<DirectionalLightShadow>().GetBuffer();
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::DirectionalLightShadowParam,				dlsBuffer.GetParamSRBuffer().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex::DirectionalLightShadowViewProjMatrix,		dlsBuffer.GetViewProjMatSRBuffer().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex::DirectionalLightShadowMapAtlas,			param.shadowSystem.renderer.GetShadowAtlasMap<DirectionalLightShadow>().GetTexture2D().GetShaderResourceView());

	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex::VXGIStaticInfoCB,						param.infoCB.staticInfoCB);
	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex::VXGIDynamicInfoCB,					param.infoCB.dynamicInfoCB);
	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex::TBRParam,								param.tbrParamCB);

	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex::ShadowGlobalParam,					param.shadowSystem.manager.GetGlobalParamCB());

	PixelShader::BindSamplerState(dx,		SamplerStateBindIndex::ShadowComprisonSamplerState,			SamplerState::ShadowGreaterEqualComp);	
	PixelShader::BindSamplerState(dx,		SamplerStateBindIndex::ShadowPointSamplerState,				SamplerState::Point);
	PixelShader::BindSamplerState(dx,		SamplerStateBindIndex::DefaultSamplerState,					SamplerState::Anisotropic);	

	UpdateConstBuffer(dx, param.startCenterWorldPos);

	GeometryShader::BindConstBuffer(dx,		ConstBufferBindIndex::VoxelizationInfoCB,					_infoCB);
	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex::VoxelizationInfoCB,					_infoCB);

	// Voxel World BoundBox에 들어오는 Mesh들만 복셀화를 한다.
	const auto& cullParam = param.cullParam;
	MeshUtility::ClassifyOpaqueMesh(_renderQ.opaqueRenderQ, cullParam.meshManager.GetOpaqueMeshPool(), cullParam.objMgr, cullParam.transformPool,
		[&worldBB = _voxeWorldBoundBox](const Mesh& mesh, const Transform&)
		{
			return worldBB.Intersects(mesh.GetBoundBox());
		}
	);

	MeshUtility::ClassifyOpaqueMesh(_renderQ.alphaTestRenderQ, cullParam.meshManager.GetAlphaTestMeshPool(), cullParam.objMgr, cullParam.transformPool,
		[&worldBB = _voxeWorldBoundBox](const Mesh& mesh, const Transform&)
		{
			return worldBB.Intersects(mesh.GetBoundBox());
		}
	);
	
	auto VoxelizeMesh = [&dx, &param](auto& renderQ)
	{
		MeshRenderer::RenderOpaqueMeshes(dx, param.meshRenderParam, DefaultRenderType::Voxelization, renderQ,
			[&dx, &materialMgr = param.materialMgr](const Mesh* mesh)
			{
				auto material	= materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID()); assert(material);

				auto diffuseMap	= material->GetTextures().Find(PhysicallyBasedMaterial::GetDiffuseMapKey()); assert(diffuseMap);
				PixelShader::BindShaderResourceView(dx, TextureBindIndex::DiffuseMap, diffuseMap->resource.GetShaderResourceView());

				auto opacityMap = material->GetTextures().Find(PhysicallyBasedMaterial::GetOpacityMapKey());
				if (opacityMap)
					PixelShader::BindShaderResourceView(dx, TextureBindIndex::OpacityMap, opacityMap->resource.GetShaderResourceView());
			},
			[&dx]()
			{
				PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::OpacityMap);
				PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DiffuseMap);
			}
		);

	};

	VoxelizeMesh(_renderQ.opaqueRenderQ);
	VoxelizeMesh(_renderQ.alphaTestRenderQ);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DirectionalLightDirXY);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DirectionalLightColor);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DirectionalLightOptionalParamIndex);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DirectionalLightShadowParam);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DirectionalLightShadowViewProjMatrix);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DirectionalLightShadowMapAtlas);

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIStaticInfoCB);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::VXGIDynamicInfoCB);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::ShadowGlobalParam);

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::ShadowComprisonSamplerState);
	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::ShadowPointSamplerState);
	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);

	dx.ReSetUAVsWithoutRenderTarget(0, ARRAYSIZE(uavs));
	dx.SetRasterizerState(RasterizerState::CWDefault);
}

void Voxelization::UpdateConstBuffer(DirectX& dx, const Vector3& startCenterPos)
{
	// Compute Voxelize Bound
	_voxeWorldBoundBox = ComputeBound(startCenterPos, _worldSize);

	InfoCBData currentVoxelizeInfo;
	currentVoxelizeInfo.voxelizeMinPos	= Vector4(	_voxeWorldBoundBox.GetMin().x,
													_voxeWorldBoundBox.GetMin().y,
													_voxeWorldBoundBox.GetMin().z,
													1.0f);

	// Update View Proj ConstBuffer
	{
		Matrix orthoProjMat = Matrix::OrthoLH(_worldSize, _worldSize, 0.0f, _worldSize);

		auto LookAt = [](const Vector3& worldPos, const Vector3& targetPos, const Vector3& up)
		{
			Matrix worldMat = Matrix::LookAtDir((targetPos - worldPos).Normalized(), &up);
			return Matrix::ComputeViewMatrix(worldMat);
		};

		const Vector3& center = _voxeWorldBoundBox.GetCenter();
		float halfWorldSize = _worldSize / 2.0f;

		Matrix viewAxisX = LookAt(center + Vector3(halfWorldSize, 0.0f, 0.0f), center, Vector3(0.0f, 1.0f, 0.0f)); //x
		Matrix viewAxisY = LookAt(center + Vector3(0.0f, halfWorldSize, 0.0f), center, Vector3(0.0f, 0.0f,-1.0f)); //y
		Matrix viewAxisZ = LookAt(center + Vector3(0.0f, 0.0f, halfWorldSize), center, Vector3(0.0f, 1.0f, 0.0f)); //z

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
		currentVoxelizeInfo.viewProjX = Matrix::Transpose(currentVoxelizeInfo.viewProjX);
		currentVoxelizeInfo.viewProjY = Matrix::Transpose(currentVoxelizeInfo.viewProjY);
		currentVoxelizeInfo.viewProjZ = Matrix::Transpose(currentVoxelizeInfo.viewProjZ);
	}

	_infoCB.UpdateSubResource(dx, currentVoxelizeInfo);
}

BoundBox Voxelization::ComputeBound(const Vector3& startCenterPos, float voxelizationSize)
{
	float halfWorldSize	= voxelizationSize / 2.0f;
	Vector3 bbMin = startCenterPos - Vector3(halfWorldSize, halfWorldSize, halfWorldSize);
	Vector3 bbMax = bbMin + Vector3(voxelizationSize, voxelizationSize, voxelizationSize);

	return BoundBox::Make(bbMin, bbMax);
}
