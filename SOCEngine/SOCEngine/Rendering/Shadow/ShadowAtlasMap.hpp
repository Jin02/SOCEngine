#pragma once

#include "DepthMap.h"
#include "RenderTexture.h"

namespace Rendering
{
	namespace Shadow
	{
		template <class ShadowType>
		class ShadowAtlasMap : public Texture::DepthMap
		{
		public:
			using Parent = Texture::DepthMap;
			using Parent::Parent;

			void Initialize(Device::DirectX& dx, const Size<uint>& size,
				uint resolution, uint capacity)
			{
				Parent::Initialize(dx, size, true, 1);

				_resolution	= resolution;
				_capacity	= capacity;
			}

			GET_CONST_ACCESSOR(Resolution,	uint,	_resolution);
			GET_CONST_ACCESSOR(Capacity,	uint,	_capacity);

		private:
			using Parent::Initialize;

		private:
			uint					_resolution		= 0;
			uint					_capacity		= 0;
		};

		class DirectionalLightShadow;
		template <> class ShadowAtlasMap<DirectionalLightShadow> : public Texture::DepthMap
		{
		public:
			using Parent = Texture::DepthMap;
			using Parent::Parent;

			void Initialize(Device::DirectX& dx, const Size<uint>& size,
				uint resolution, uint capacity)
			{
				Parent::Initialize(dx, size, true, 1);

				_viewDepthMap.Initialize(dx, size, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN, 0, 1, 1);

				_resolution	= resolution;
				_capacity	= capacity;
			}

			void Destroy()
			{
				Parent::Destroy();
				_viewDepthMap.Destroy();

				_resolution	= 0;
				_capacity	= 0;
			}

			GET_CONST_ACCESSOR(Resolution,	uint,						_resolution);
			GET_CONST_ACCESSOR(Capacity,	uint,						_capacity);
			GET_ACCESSOR_PTR(ViewDepthMap,	Texture::RenderTexture,		_viewDepthMap);

		private:
			using Parent::Initialize;
			using Parent::Destroy;

		private:
			Texture::RenderTexture	_viewDepthMap;

			uint					_resolution		= 0;
			uint					_capacity		= 0;
		};


	}
}