#pragma once

#include "AnisotropicVoxelMapAtlas.h"
#include <functional>
#include "Vector3.h"
#include "ConstBuffer.h"
#include "MeshCamera.h"
#include "ComputeShader.h"

namespace Rendering
{
	namespace GI
	{
		class Voxelization
		{
		public:
			struct Info
			{
				float voxelizeSize;
				uint  dimension;
			};

			struct InfoCBData : public Info
			{
				Math::Vector3	voxelizeMinPos;
				float			voxelSize;
				uint			currentCascade;
				float			dummy;
			};

			struct ViewProjAxisesCBData
			{
				Math::Matrix viewProjX;
				Math::Matrix viewProjY;
				Math::Matrix viewProjZ;
			};

			enum class BindIndex : uint
			{
				Albedo		= 0,
				Normal		= 1,
				Emission	= 2
			};

		private:
			AnisotropicVoxelMapAtlas*	_voxelAlbedoMapAtlas;
			AnisotropicVoxelMapAtlas*	_voxelNormalMapAtlas;
			AnisotropicVoxelMapAtlas*	_voxelEmissionMapAtlas;

			Buffer::ConstBuffer*	_infoConstBuffer;
			Buffer::ConstBuffer*	_viewProjAxisesConstBuffer;

			InfoCBData				_initVoxelizationInfo;

			Math::Matrix			_prevStaticMeshVoxelizeViewMat;

			uint					_maxNumOfCascade;

		private:
			GPGPU::DirectCompute::ComputeShader*	_clearVoxelMapCS;

		public:
			Voxelization();
			~Voxelization();

		public:
			void Initialize(uint cascades, float minWorldSize = 4.0f, uint dimension = 256);
			void Destroy();

		public:
			void ClearZeroVoxelMap(const Device::DirectX*& dx);
			void Voxelize(const Device::DirectX*& dx, const Camera::MeshCamera*& camera, const Manager::RenderManager*& renderManager, bool onlyStaticMesh);

		public:
			void UpdateInitVoxelizationInfo(const Info& info);
		};
	}
}