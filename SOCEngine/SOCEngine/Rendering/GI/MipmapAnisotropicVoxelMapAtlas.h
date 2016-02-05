#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"
#include "VoxelMap.h"
#include "InjectRadiance.h"

namespace Rendering
{
	namespace GI
	{
		class MipmapAnisotropicVoxelMapAtlas
		{
		public:
			struct InfoCB
			{
				uint sourceDimension;
				uint currentCascade;
				uint dummy1, dummy2;
			};

		private:
			GPGPU::DirectCompute::ComputeShader*		_shader;
			Buffer::ConstBuffer*						_infoCB;

		public:
			MipmapAnisotropicVoxelMapAtlas();
			~MipmapAnisotropicVoxelMapAtlas();

		public:
			void Initialize(const GlobalInfo& giInfo);
			void Mipmapping(const Device::DirectX* dx, const VoxelMap* sourceColorMap, uint maxNumOfCascade);
			void Destroy();
		};
	}
}