#pragma once

#include "DirectX.h"

namespace Rendering
{
	namespace View
	{
		class UnorderedAccessView final
		{
		public:
			UnorderedAccessView();
			UnorderedAccessView(const DXResource<ID3D11UnorderedAccessView>& uav);

			GET_CONST_ACCESSOR(View, DXResource<ID3D11UnorderedAccessView>, _uav);

			template <class DXResourceType>
			void Initialize(Device::DirectX& dx, DXGI_FORMAT format, uint numElements, DXResourceType& resource, D3D11_UAV_DIMENSION viewDimension, uint tex3dMipSlice = 0, uint tex3dWSize = 0, uint bufferFlags = 0)
			{				
				Initialize(dx, format, numElements, resource.GetRaw(), viewDimension, tex3dMipSlice, tex3dWSize, bufferFlags);
			}
			void Destroy();

		private:
			void Initialize(Device::DirectX& dx, DXGI_FORMAT format, uint numElements, ID3D11Resource* resource, D3D11_UAV_DIMENSION viewDimension, uint tex3dMipSlice = 0, uint tex3dWSize = 0, uint bufferFlags = 0);

		private:
			DXResource<ID3D11UnorderedAccessView>		_uav;
		};
	}
}
