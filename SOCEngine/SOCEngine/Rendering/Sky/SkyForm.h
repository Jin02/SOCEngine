#pragma once

#include "Mesh.h"
#include "Material.h"
#include "CameraForm.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"

namespace Rendering
{
	namespace Sky
	{
		class SkyForm
		{
		public:
			enum class Type
			{
				Unknown,
				Box,
				Dome
			};

			struct SkyMapInfoCBData
			{
				float	maxMipCount;
				uint	isSkyLightOn;
				uint	isDynamicSkyLight;
				float	blendFraction;
			};

		private:
			Geometry::Mesh*				_mesh;
			Buffer::ConstBuffer*		_skyMapInfoCB;

			SkyMapInfoCBData			_prevCBData;

		protected:
			float						_maxMipCount;
			bool						_isSkyLightOn;
			bool						_isDynamicSkyLight;
			float						_blendFraction;

		public:
			SkyForm();
			virtual ~SkyForm();

		protected:
			void Initialize(const Material* skyMaterial);
			void _Render(const Device::DirectX* dx, const Texture::RenderTexture* renderTarget, const Texture::DepthBuffer* opaqueDepthBuffer);

		public:
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera, const Texture::RenderTexture* renderTarget, const Texture::DepthBuffer* opaqueDepthBuffer) = 0;
			virtual void Destroy();

			void UpdateConstBuffer(const Device::DirectX* dx);

		public:
			const Texture::Texture2D* GetSkyCubeMap() const;

			GET_SET_ACCESSOR(BlendFraction,		float,					_blendFraction);
			GET_SET_ACCESSOR(IsSkyOn,			bool,					_isSkyLightOn);
			GET_ACCESSOR(SkyMapInfoConstBuffer,	Buffer::ConstBuffer*,	_skyMapInfoCB);
		};
	}
}