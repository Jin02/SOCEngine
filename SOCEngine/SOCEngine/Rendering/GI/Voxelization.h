#pragma once

#include "VoxelMap.h"
#include <functional>
#include "Vector3.h"
#include "ConstBuffer.h"
#include "MeshCamera.h"
#include "ComputeShader.h"
#include "GlobalIlluminationCommon.h"

#include "RAWBuffer.h"

namespace Core
{
	class Scene;
}

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
//				Math::Matrix	worldToVoxel;

				Math::Vector3	voxelizeMinPos;
				uint			currentCascade;
			};

		private:
			//VoxelMap*								_voxelAlbedoMapAtlas;
			//VoxelMap*								_voxelNormalMapAtlas;
			//VoxelMap*								_voxelEmissionMapAtlas;

			Buffer::RAWBuffer*						_voxelAlbedoMapAtlas;
			Buffer::RAWBuffer*						_voxelNormalMapAtlas;
			Buffer::RAWBuffer*						_voxelEmissionMapAtlas;

			Math::Matrix							_prevStaticMeshVoxelizeViewMat;

			std::vector<Buffer::ConstBuffer*>		_constBuffers;
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

		public:
			Voxelization();
			~Voxelization();

		private:
			void InitializeClearVoxelMap(uint dimension, uint maxNumOfCascade);
			void UpdateConstBuffer(const Device::DirectX*& dx, uint currentCascade, const Math::Vector3& camWorldPos, float initWorldSize, float dimension);
			void ClearZeroVoxelMap(const Device::DirectX*& dx);

		public:
			void Initialize(uint maxNumOfCascade, uint dimension);
			void Voxelize(const Device::DirectX*& dx,
				const Camera::MeshCamera*& camera, const Core::Scene* scene,
				float maxNumOfCascade, float initWorldSize, const VoxelMap* injectionColorMap,
				const Buffer::ConstBuffer* giGlobalStaticInfoCB, const Buffer::ConstBuffer* giGlobalDynamicInfoCB,
				bool onlyStaticMesh);

			void Destroy();

		public:
			static void ComputeVoxelViewMatrix(Math::Matrix& outMat, uint currentCascade, const Math::Vector3& camWorldPos, float initVoxelizeSize);
			static void ComputeBound(Math::Vector3* outMin, Math::Vector3* outMid, Math::Vector3* outMax, float* outWorldSize,
									 uint currentCascade, const Math::Vector3& camWorldPos, float initVoxelizeSize);

		public:
			GET_ACCESSOR(ConstBuffers,							const std::vector<Buffer::ConstBuffer*>&,	_constBuffers);
			GET_ACCESSOR(AnisotropicVoxelAlbedoMapAtlas,		const Buffer::RAWBuffer*,					_voxelAlbedoMapAtlas);
			GET_ACCESSOR(AnisotropicVoxelNormalMapAtlas,		const Buffer::RAWBuffer*,					_voxelNormalMapAtlas);
			GET_ACCESSOR(AnisotropicVoxelEmissionMapAtlas,		const Buffer::RAWBuffer*,					_voxelEmissionMapAtlas);
		};
	}
}