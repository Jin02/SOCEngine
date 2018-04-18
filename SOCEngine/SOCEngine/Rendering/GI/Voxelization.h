#pragma once

#include "GlobalDefine.h"

#include "VoxelMap.h"

#include "Vector3.h"
#include "Vector4.h"

#include "ConstBuffer.h"
#include "ComputeShader.h"
#include "VXGICommon.h"
#include "GPURawBuffer.h"

#include "ShaderManager.h"
#include "LightManager.h"
#include "ShadowAtlasMapRenderer.h"

#include "TileBasedShadingHeader.h"
#include "MeshRenderer.h"
#include "RendererCommon.h"

namespace Rendering
{
	namespace GI
	{
		class Voxelization
		{
		public:
			struct InfoCBData
			{
				Math::Matrix	viewProjX;
				Math::Matrix	viewProjY;
				Math::Matrix	viewProjZ;
				
				Math::Vector4	voxelizeMinPos;

				InfoCBData() = default;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint dimension, float voxelSize);
			void UpdateConstBuffer(Device::DirectX& dx, const Math::Vector3& startCenterPos);

			GET_CONST_ACCESSOR(InfoCB,					const auto&,	_infoCB);
			GET_CONST_ACCESSOR(VoxelAlbedoRawBuffer,	const auto&,	_voxelAlbedoRawBuffer);
			GET_CONST_ACCESSOR(VoxelNormalRawBuffer,	const auto&,	_voxelNormalRawBuffer);
			GET_CONST_ACCESSOR(VoxelEmissionRawBuffer,	const auto&,	_voxelEmissionRawBuffer);

			struct Param
			{
				const VXGIInfoCB&						infoCB;
				const Manager::LightManager&			lightMgr;
				const Renderer::ShadowSystem&			shadowParam;
				const Renderer::TBRParamCB&				tbrParamCB;
				const Renderer::CullingParam&			cullParam;
				const Renderer::MeshRenderer::Param&	meshRenderParam;
				const Manager::MaterialManager&			materialMgr;
			};
			void Voxelize(Device::DirectX& dx, VoxelMap& outDLInjectVoxelMap, const Param&& param);

		private:
			void ClearVoxelMap(Device::DirectX& dx, const Buffer::ExplicitConstBuffer<VXGIStaticInfo>& vxgiStaticInfoCB);
			static Intersection::BoundBox ComputeBound(const Math::Vector3& startCenterPos, float voxelizationSize);

		private:
			Buffer::GPURawBuffer						_voxelAlbedoRawBuffer;
			Buffer::GPURawBuffer						_voxelNormalRawBuffer;
			Buffer::GPURawBuffer						_voxelEmissionRawBuffer;

			Buffer::ExplicitConstBuffer<InfoCBData>		_infoCB;
			Shader::ComputeShader						_clearVoxelRawMapCS;

			Renderer::TempRenderQueue					_renderQ;
			Intersection::BoundBox						_voxeWorldBoundBox;

			float										_dimension = 0.0f;
			float										_worldSize = 0.0f;
		};
	}
}
