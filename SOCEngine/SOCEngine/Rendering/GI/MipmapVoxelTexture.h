#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"
#include "VoxelMap.h"
#include "InjectRadiance.h"

namespace Rendering
{
	namespace GI
	{
		class MipmapVoxelTexture
		{
		public:
			struct InfoCB
			{
				uint sourceDimension;
				uint dummy1, dummy2, dummy3;
			};

		private:
			GPGPU::DirectCompute::ComputeShader*		_shader;
			Buffer::ConstBuffer*						_infoCB;

		public:
			MipmapVoxelTexture();
			~MipmapVoxelTexture();

		public:
			void Initialize();
			void Mipmapping(const Device::DirectX* dx, const VoxelMap* sourceColorMap);
			void Destroy();
		};
	}
}