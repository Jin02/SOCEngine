#pragma once

#include "AnisotropicVoxelMap.h"
#include <functional>
#include "Vector3.h"
#include "ConstBuffer.h"
#include "MeshCamera.h"

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
				float voxelSize;
				uint  currentCascade;
			};

			struct ViewProjAxisesCBData
			{
				Math::Matrix viewProjX;
				Math::Matrix viewProjY;
				Math::Matrix viewProjZ;
			};

			enum class UAVBindSlotIndex : uint
			{
				AnisotropicVoxelMap_PosX = 0,
				AnisotropicVoxelMap_NegX = 1,
				AnisotropicVoxelMap_PosY = 2,
				AnisotropicVoxelMap_NegY = 3,
				AnisotropicVoxelMap_PosZ = 4,
				AnisotropicVoxelMap_NegZ = 5,
				AnisotropicVoxelMap_StartOffset = AnisotropicVoxelMap_PosX
			};

		private:
			std::vector<AnisotropicVoxelMap*>	_voxelMaps;

			Buffer::ConstBuffer*	_infoConstBuffer;
			Buffer::ConstBuffer*	_viewProjAxisesConstBuffer;

			InfoCBData				_initVoxelizationInfo;
			bool					_changedInitVoxelizationInfo;

			Math::Matrix			_prevViewMat;

			uint					_numOfCascades;

		public:
			Voxelization();
			~Voxelization();

		public:
			void Initialize(uint cascades, float minWorldSize = 4.0f, uint dimension = 256);
			void Destroy();

		public:
			void Clear(const Device::DirectX*& dx);
			void Voxelize(const Device::DirectX*& dx, const Camera::MeshCamera*& camera, const Manager::RenderManager*& renderManager);

		public:
			void UpdateInitVoxelizationInfo(const Info& info);
		};
	}
}