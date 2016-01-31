#pragma once

#include "VoxelMap.h"
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
				Math::Matrix	voxelView;
				Math::Matrix	voxelViewProj;

				Math::Vector3	voxelizeMinPos;
				uint			currentCascade;
			};

		private:
			VoxelMap*								_voxelAlbedoMapAtlas;
			VoxelMap*								_voxelNormalMapAtlas;
			VoxelMap*								_voxelEmissionMapAtlas;

			Math::Matrix							_prevStaticMeshVoxelizeViewMat;

			std::vector<Buffer::ConstBuffer*>		_constBuffers;
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

		public:
			Voxelization();
			~Voxelization();

		private:
			void InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade);
			void UpdateConstBuffer(const Device::DirectX*& dx, uint currentCascade, const Math::Vector3& camWorldPos, const GlobalInfo& globalInfo, float dimension);
			void ClearZeroVoxelMap(const Device::DirectX*& dx);

		public:
			void Initialize(const GlobalInfo& globalInfo);			
			void Voxelize(const Device::DirectX*& dx, const Camera::MeshCamera*& camera, const Manager::RenderManager*& renderManager, const GlobalInfo& globalInfo, bool onlyStaticMesh);
			void Destroy();

		public:
			static void ComputeVoxelViewMatrix(Math::Matrix& outMat, uint currentCascade, const Math::Vector3& camWorldPos, float initVoxelizeSize);
			static void ComputeBound(Math::Vector3* outMin, Math::Vector3* outMid, Math::Vector3* outMax, float* outWorldSize,
									 uint currentCascade, const Math::Vector3& camWorldPos, float initVoxelizeSize);

		public:
			GET_ACCESSOR(ConstBuffers,							const std::vector<Buffer::ConstBuffer*>&,	_constBuffers);
			GET_ACCESSOR(AnisotropicVoxelAlbedoMapAtlas,		const VoxelMap*,							_voxelAlbedoMapAtlas);
			GET_ACCESSOR(AnisotropicVoxelNormalMapAtlas,		const VoxelMap*,							_voxelNormalMapAtlas);
			GET_ACCESSOR(AnisotropicVoxelEmissionMapAtlas,		const VoxelMap*,							_voxelEmissionMapAtlas);
		};
	}
}