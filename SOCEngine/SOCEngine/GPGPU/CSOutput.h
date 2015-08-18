#pragma once

#include "DirectX.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class CSOutput
		{
		private:
			ID3D11UnorderedAccessView*		_uav;

		public:
			CSOutput();
			virtual ~CSOutput();

		public:
			void Initialize(DXGI_FORMAT format, uint numElements, ID3D11Resource* resource);
			void Destroy();

		public:
			GET_ACCESSOR(UnorderedAccessView, ID3D11UnorderedAccessView*, _uav);
		};
	}
}
