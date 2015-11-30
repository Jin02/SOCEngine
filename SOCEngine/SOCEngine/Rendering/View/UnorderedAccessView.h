#pragma once

#include "DirectX.h"

namespace Rendering
{
	namespace View
	{
		class UnorderedAccessView
		{
		private:
			ID3D11UnorderedAccessView*		_uav;

		public:
			UnorderedAccessView();
			virtual ~UnorderedAccessView();

		public:
			void Initialize(DXGI_FORMAT format, uint numElements, ID3D11Resource* resource, D3D11_UAV_DIMENSION viewDimension);
			void Destroy();

		public:
			GET_ACCESSOR(View, ID3D11UnorderedAccessView*, _uav);
		};
	}
}
