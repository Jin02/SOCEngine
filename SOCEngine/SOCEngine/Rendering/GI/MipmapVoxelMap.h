#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "VXGICommon.h"
#include "VoxelMap.h"
#include "ShaderManager.h"

namespace Rendering
{
	namespace GI
	{
		class MipmapVoxelMap
		{
		public:
			struct InfoCBData
			{
				uint sourceDimension;
				uint dummy1, dummy2, dummy3;
			};

		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Destroy();

			void Mipmapping(Device::DirectX& dx, const VoxelMap& sourceColorMap, VoxelMap& outAnisotropicMap);

		private:
			Shader::ComputeShader						_baseMipmap;
			Shader::ComputeShader						_anisotropicMipmap;

			Buffer::ExplicitConstBuffer<InfoCBData>		_infoCB;
		};
	}
}