#pragma once

#include "DepthBuffer.h"

namespace Rendering
{
	namespace Shadow
	{
		class ShadowAtlasMap
		{
		public:
			void Initialize(Device::DirectX& dx, uint lightCount, uint resolution, bool isPointLight = false);
			void ReSize(Device::DirectX& dx, uint lightCount, uint resolution);
			void Destroy();

			GET_ACCESSOR(DepthBuffer,			auto&,	_atlasMap);
			GET_CONST_ACCESSOR(Resolution,		uint,	_resolution);
			GET_CONST_ACCESSOR(Capacity,		uint,	_capacity);
			GET_CONST_ACCESSOR(IsPointLight,	bool,	_isPointLight);

		protected:
			bool ChangedAtlasSize(uint lightCount, uint resolution);
			static uint Next2Squre(uint value);

		protected:
			Texture::DepthBuffer	_atlasMap;
			uint					_resolution		= 0;
			uint					_capacity		= 0;
			bool					_isPointLight	= false;
		};
	}
}