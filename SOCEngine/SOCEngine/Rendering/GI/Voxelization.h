#pragma once

#include "GlobalDefine.h"

#include "VoxelMap.h"
#include <functional>

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
#ifdef USE_BLOATING_IN_VOXELIZATION_PASS 
				Math::Matrix	viewProjX_inv;
				Math::Matrix	viewProjY_inv;
				Math::Matrix	viewProjZ_inv;				
#endif				
				Math::Vector4	voxelizeMinPos;

				InfoCBData() = default;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, uint dimension, float voxelSize);

			void Voxelize(Device::DirectX& dx, VoxelMap& outDLInjectVoxelMap,
				const Math::Vector3& startMinWorldPos, VXGIInfoCB& infoCB,
				Manager::LightManager& lightMgr, ShadowSystemParam& shadowSystem, Renderer::TBRParamCB& tbrParamCB, Renderer::MeshRenderer& meshRenderer);

			void UpdateConstBuffer(Device::DirectX& dx, const Math::Vector3& startCenterPos);

			struct ComputeBoundResult { Math::Vector3 bbMin, bbMid, bbMax; };
			static ComputeBoundResult ComputeBound(const Math::Vector3& startCenterPos, float voxelizationSize);

			GET_ACCESSOR(InfoCB, auto&, _infoCB);
			GET_ACCESSOR(VoxelAlbedoRawBuffer, auto&, _voxelAlbedoRawBuffer);
			GET_ACCESSOR(VoxelNormalRawBuffer, auto&, _voxelNormalRawBuffer);
			GET_ACCESSOR(VoxelEmissionRawBuffer, auto&, _voxelEmissionRawBuffer);

		private:
			void ClearVoxelMap(Device::DirectX& dx, Buffer::ExplicitConstBuffer<VXGIStaticInfo>& vxgiStaticInfoCB);

		private:
			Buffer::GPURawBuffer						_voxelAlbedoRawBuffer;
			Buffer::GPURawBuffer						_voxelNormalRawBuffer;
			Buffer::GPURawBuffer						_voxelEmissionRawBuffer;

			Buffer::ExplicitConstBuffer<InfoCBData>		_infoCB;
			Shader::ComputeShader						_clearVoxelRawMapCS;

			float										_dimension = 0.0f;
			float										_worldSize = 0.0f;
		};
	}
}
