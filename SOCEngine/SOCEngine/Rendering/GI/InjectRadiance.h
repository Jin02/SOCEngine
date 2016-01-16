#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "AnisotropicVoxelMapAtlas.h"
#include "GlobalIlluminationCommon.h"
#include "DirectX.h"
#include "Scene.h"

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

				bool IsValid() const
				{
					return globalInfo && giInfoConstBuffer && albedoMap && normalMap && emissionMap;
				}
			};

		private:
			AnisotropicVoxelMapAtlas*							_colorMap;

		protected:
			GPGPU::DirectCompute::ComputeShader*				_shader;

		protected:
			InjectRadiance();
			~InjectRadiance();

		protected:
			void Initialize(const std::string& fileName, const InitParam& param);
			void Destroy();

			void Dispath(const Device::DirectX* dx,
						 const std::vector<Buffer::ConstBuffer*>& voxelizationInfoConstBuffers);

		public:
			GET_ACCESSOR(ColorMap, const AnisotropicVoxelMapAtlas*, _colorMap);
		};
	}
}