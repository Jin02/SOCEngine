#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VoxelMap.h"
#include "GlobalIlluminationCommon.h"
#include "DirectX.h"
#include "ShadowRenderer.h"
#include "Voxelization.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadiance
		{
		public:
			struct InitParam
			{
				uint								dimension;
				const Buffer::ConstBuffer*			giStaticInfoCB;
				const Buffer::ConstBuffer*			giDynamicInfoCB;
				const VoxelMap*						outColorMap;
				const Voxelization*					voxelization;

				bool IsValid() const
				{
					return dimension && giStaticInfoCB && giDynamicInfoCB && voxelization && outColorMap;
				}
			};

		protected:
			GPGPU::DirectCompute::ComputeShader*				_shader;

		protected:
			InjectRadiance();
			~InjectRadiance();

		protected:
			void Initialize(const std::string& fileName, const InitParam& param);
			void Dispath(const Device::DirectX* dx,
						 const std::vector<Buffer::ConstBuffer*>& voxelizationInfoConstBuffers);

		public:
			void Destroy();
		};
	}
}