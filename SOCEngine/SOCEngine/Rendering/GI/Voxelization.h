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
				AlbedoUAV			= 0,
				NormalUAV			= 1,
				EmissionUAV			= 2,
				InfoCB				= 0,
				ViewProjAxisesCB	= 1
			};

		private:
			AnisotropicVoxelMapAtlas*							_voxelAlbedoMapAtlas;
			AnisotropicVoxelMapAtlas*							_voxelNormalMapAtlas;
			AnisotropicVoxelMapAtlas*							_voxelEmissionMapAtlas;

			Buffer::ConstBuffer*								_infoConstBuffer;
			Buffer::ConstBuffer*								_viewProjAxisesConstBuffer;

			InfoCBData											_initVoxelizationInfo;

			Math::Matrix										_prevStaticMeshVoxelizeViewMat;

			uint												_maxNumOfCascade;

			std::vector<Shader::ShaderForm::InputConstBuffer>	_inputConstBuffers;

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
			void ComputeVoxelVolumeProjMatrix(Math::Matrix& outMat, uint currentCascade, const Math::Vector3& camWorldPos) const;
			void ComputeBound(Math::Vector3* outMin, Math::Vector3* outMid, Math::Vector3* outMax, float* outWorldSize, Math::Vector3* outVoxelizeMinPos, uint currentCascade, const Math::Vector3& camWorldPos) const;

		public:
			void UpdateInitVoxelizationInfo(const Info& info);
		};
	}
}