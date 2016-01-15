#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "AnisotropicVoxelMapAtlas.h"
#include "GlobalIlluminationCommon.h"

namespace Rendering
{
	namespace GI
	{
		class InjectRadiance
		{
		public:
			struct InitParam
			{
				const GlobalInfo*					globalInfo;
				const Buffer::ConstBuffer*			giInfoConstBuffer;
				const AnisotropicVoxelMapAtlas*		albedoMap;
				const AnisotropicVoxelMapAtlas*		normalMap;
				const AnisotropicVoxelMapAtlas*		emissionMap;

				bool IsInvalid() const
				{
					return globalInfo && giInfoConstBuffer && albedoMap && normalMap && emissionMap;
				}
			};

		private:
			AnisotropicVoxelMapAtlas*							_colorMap;

		protected:
			GPGPU::DirectCompute::ComputeShader*				_shader;

		public:
			InjectRadiance();
			~InjectRadiance();

		protected:
			void Initialize(const std::string& fileName,
							const GPGPU::DirectCompute::ComputeShader::ThreadGroup& threadGroup,
							const InitParam& param);

			void Dispath(const Device::DirectX* dx,
						 const std::vector<Buffer::ConstBuffer*>& voxelizationInfoConstBuffers);
			void Destroy();
		};
	}
}