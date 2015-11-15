#pragma once

#include "DirectX.h"

namespace Rendering
{
	namespace Shader
	{
		class UnorderedAccessView
		{
		public:
			enum class Type
			{
				Buffer,
				Texture
			};

		private:
			ID3D11UnorderedAccessView*		_uav;

		public:
			UnorderedAccessView();
			virtual ~UnorderedAccessView();

		public:
			void Initialize(DXGI_FORMAT format, uint numElements, ID3D11Resource* resource, Type type);
			void Destroy();

		public:
			GET_ACCESSOR(UnorderedAccessView, ID3D11UnorderedAccessView*, _uav);
		};
	}
}
