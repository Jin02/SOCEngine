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
		class MipmapVoxelMap
		{
		public:
			struct InfoCB
			{
				uint sourceDimension;
				uint dummy1, dummy2, dummy3;
			};

		private:
			GPGPU::DirectCompute::ComputeShader*		_baseMipmap;
			GPGPU::DirectCompute::ComputeShader*		_anisotropicMipmap;

			Buffer::ConstBuffer*						_infoCB;

		public:
			MipmapVoxelMap();
			~MipmapVoxelMap();

		public:
			void Initialize();
			void Mipmapping(const Device::DirectX* dx, const VoxelMap* sourceColorMap, const VoxelMap* anisotropicMap);
			void Destroy();
		};
	}
}