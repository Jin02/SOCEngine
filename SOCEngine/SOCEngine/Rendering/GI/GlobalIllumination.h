#pragma once

#include "ComputeShader.h"
#include "ConstBuffer.h"
#include "GlobalIlluminationCommon.h"

#include "Voxelization.h"

#include "InjectRadianceFromDirectionalLIght.h"
#include "InjectRadianceFromPointLIght.h"
#include "InjectRadianceFromSpotLIght.h"

#include "MipmapAnisotropicVoxelMapAtlas.h"

#include "CameraForm.h"

namespace Rendering
{
	namespace GI
	{
		class GlobalIllumination
		{
		private:
			GlobalInfo								_globalInfo;
			Buffer::ConstBuffer*					_giGlobalInfoCB;

			Voxelization*							_voxelization;

			InjectRadianceFromDirectionalLIght*		_injectDirectionalLight;
			InjectRadianceFromPointLIght*			_injectPointLight;
			InjectRadianceFromSpotLIght*			_injectSpotLight;

			MipmapAnisotropicVoxelMapAtlas*			_mipmap;

		public:
			GlobalIllumination();
			~GlobalIllumination();

		public:
			void Initialize(const Device::DirectX* dx, uint dimension = 256, float minWorldSize = 4.0f);
			void Run(const Device::DirectX* dx, const Camera::CameraForm*& camera, const Core::Scene* curScene);
			void Destroy();
		};
	}
}