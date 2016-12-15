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
			enum class Type { Common, Atmospheric };

			struct SkyMapInfoCBData
			{
				float	maxMipCount;
				uint	isSkyLightOn;
				uint	isDynamicSkyLight;
				float	blendFraction;
			};

		private:
			Buffer::ConstBuffer*		_skyMapInfoCB;
			Geometry::MeshFilter*		_meshFilter;

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
			virtual void Render(const Device::DirectX* dx, const Camera::CameraForm* camera,
				const Texture::RenderTexture* out, const Texture::DepthBuffer* depthBuffer,
				const Manager::LightManager* lightMgr) = 0;
			virtual void Render(const Device::DirectX* dx, const Camera::ReflectionProbe* probe,
				const Texture::TextureCube* out, const Texture::DepthBufferCube* depthBuffer,
				const Manager::LightManager* lightMgr) = 0;

			virtual void Destroy();

			void UpdateParam(const Device::DirectX* dx);

		protected:
			GET_ACCESSOR(MeshFilter,			const Geometry::MeshFilter*,	_meshFilter);

		public:
			GET_SET_ACCESSOR(BlendFraction,		float,							_blendFraction);
			GET_SET_ACCESSOR(IsSkyOn,			bool,							_isSkyLightOn);
			GET_ACCESSOR(SkyMapInfoConstBuffer,	Buffer::ConstBuffer*,			_skyMapInfoCB);
			GET_ACCESSOR(Type,					Type,							_type);
		};
	}
}