#pragma once

#include "VoxelMap.h"
#include <functional>
#include "Vector3.h"
#include "ConstBuffer.h"
#include "MeshCamera.h"
#include "ComputeShader.h"
#include "VXGICommon.h"
#include "RawBuffer.h"

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
#ifdef USE_BLOATING_IN_VOXELIZATION_PASS 
				Math::Matrix	viewProjX_inv;
				Math::Matrix	viewProjY_inv;
				Math::Matrix	viewProjZ_inv;				
#endif				
				Math::Vector4	voxelizeMinPos;
			};

		private:
			Buffer::RawBuffer*						_voxelAlbedoRawBuffer;
			Buffer::RawBuffer*						_voxelNormalRawBuffer;
			Buffer::RawBuffer*						_voxelEmissionRawBuffer;

			Math::Matrix							_prevStaticMeshVoxelizeViewMat;

			Buffer::ConstBuffer*					_infoCB;
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

		public:
			Voxelization();
			~Voxelization();

		private:
			void InitializeClearVoxelMap(uint dimension);
			void UpdateConstBuffer(const Device::DirectX*& dx, const Math::Vector3& startMinPos, float voxelSize, float dimension);
			void ClearZeroVoxelMap(const Device::DirectX*& dx, const Buffer::ConstBuffer* vxgiStaticInfoCB);

		public:
			void Initialize(uint dimension);
			void Voxelize(const Device::DirectX*& dx, const Math::Vector3& startCenterWorldPos, const Core::Scene* scene, float dimension, float voxelSize, const VoxelMap* injectionColorMap,
				const Buffer::ConstBuffer* vxgiStaticInfoCB, const Buffer::ConstBuffer* vxgiDynamicInfoCB);
			void Destroy();

		public:
			static void ComputeBound(Math::Vector3* outMin, Math::Vector3* outMid, Math::Vector3* outMax, float* outWorldSize, const Math::Vector3& startCenterPos, float voxelizationSize);

		public:
			GET_ACCESSOR(InfoCB,					const Buffer::ConstBuffer*,					_infoCB);
			GET_ACCESSOR(VoxelAlbedoRawBuffer,		const Buffer::RawBuffer*,					_voxelAlbedoRawBuffer);
			GET_ACCESSOR(VoxelNormalRawBuffer,		const Buffer::RawBuffer*,					_voxelNormalRawBuffer);
			GET_ACCESSOR(VoxelEmissionRawBuffer,	const Buffer::RawBuffer*,					_voxelEmissionRawBuffer);
		};
	}
}
