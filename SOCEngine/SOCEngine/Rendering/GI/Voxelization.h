#pragma once

#include "AnisotropicVoxelMapAtlas.h"
#include <functional>
#include "Vector3.h"
#include "ConstBuffer.h"
#include "MeshCamera.h"
#include "ComputeShader.h"
#include "GlobalIlluminationCommon.h"

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

				Math::Vector3	voxelizeMinPos;
				uint			currentCascade;

				float			voxelizeSize;
				float			voxelSize;

				float dummy1, dummy2;
			};

		private:
			AnisotropicVoxelMapAtlas*							_voxelAlbedoMapAtlas;
			AnisotropicVoxelMapAtlas*							_voxelNormalMapAtlas;
			AnisotropicVoxelMapAtlas*							_voxelEmissionMapAtlas;

			Math::Matrix										_prevStaticMeshVoxelizeViewMat;

			std::vector<Buffer::ConstBuffer*>					_constBuffers;
			GPGPU::DirectCompute::ComputeShader*				_clearVoxelMapCS;

		public:
			Voxelization();
			~Voxelization();

		private:
			void InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade);
			void UpdateConstBuffer(const Device::DirectX*& dx, uint currentCascade, const Math::Vector3& camWorldPos, const GlobalInfo& globalInfo, float dimension, float camNear, float camFar);

		public:
			void Initialize(uint cascades, GlobalInfo& outGlobalInfo, float minWorldSize = 4.0f, uint dimension = 256);			
			void Destroy();

		public:
			void ClearZeroVoxelMap(const Device::DirectX*& dx);
			void Voxelize(const Device::DirectX*& dx, const Camera::MeshCamera*& camera, const Manager::RenderManager*& renderManager, const GlobalInfo& globalInfo, bool onlyStaticMesh);
			static void ComputeVoxelVolumeProjMatrix(Math::Matrix& outMat, uint currentCascade, const Math::Vector3& camWorldPos, float initVoxelizeSize);
			static void ComputeBound(Math::Vector3* outMin, Math::Vector3* outMid, Math::Vector3* outMax, float* outWorldSize,
									 uint currentCascade, const Math::Vector3& camWorldPos, float initVoxelizeSize);

		public:
			GET_ACCESSOR(ConstBuffers,							const std::vector<Buffer::ConstBuffer*>&,	_constBuffers);
			GET_ACCESSOR(AnisotropicVoxelAlbedoMapAtlas,		const AnisotropicVoxelMapAtlas*,			_voxelAlbedoMapAtlas);
			GET_ACCESSOR(AnisotropicVoxelNormalMapAtlas,		const AnisotropicVoxelMapAtlas*,			_voxelNormalMapAtlas);
			GET_ACCESSOR(AnisotropicVoxelEmissionMapAtlas,		const AnisotropicVoxelMapAtlas*,			_voxelEmissionMapAtlas);
		};
	}
}