#pragma once

#include "Mesh.h"
#include "Material.h"
#include "CameraForm.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"

#include "DepthBufferCube.h"
#include "TextureCube.h"
#include "ReflectionProbe.h"

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
			Geometry::MeshFilter*		_meshFilter;
			Buffer::ConstBuffer*		_skyMapInfoCB;

			SkyMapInfoCBData			_prevCBData;

		protected:
			float						_maxMipCount;
			bool						_isSkyLightOn;
			bool						_isDynamicSkyLight;
			float						_blendFraction;
			Type						_type;

		public:
			SkyForm(Type type);
			virtual ~SkyForm();

		protected:
			void Initialize();
			void _Render(const Device::DirectX* dx, const Material* material, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);

		public:
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera, const Texture::RenderTexture* renderTarget, const Texture::DepthBuffer* opaqueDepthBuffer) = 0;
			virtual void Render(const Device::DirectX* dx, const Camera::ReflectionProbe* probe, const Texture::TextureCube* renderTarget, const Texture::DepthBufferCube* opaqueDepthBuffer) = 0;

			virtual void Destroy();

			void UpdateConstBuffer(const Device::DirectX* dx);

		public:
			GET_SET_ACCESSOR(BlendFraction,		float,					_blendFraction);
			GET_SET_ACCESSOR(IsSkyOn,			bool,					_isSkyLightOn);
			GET_ACCESSOR(SkyMapInfoConstBuffer,	Buffer::ConstBuffer*,	_skyMapInfoCB);
			GET_ACCESSOR(Type,					Type,					_type);
		};
	}
}