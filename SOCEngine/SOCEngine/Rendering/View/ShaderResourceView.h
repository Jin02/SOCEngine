#pragma once

#include "Vector3.h"
#include "Common.h"
#include "DirectX.h"
#include "BaseBuffer.h"
#include <memory>

namespace Rendering
{
	namespace View
	{
		class ShaderResourceView final
		{
		public:
			ShaderResourceView() = default;
			ShaderResourceView(const DXSharedResource<ID3D11ShaderResourceView>& srv) : _srv(srv) {}

			GET_CONST_ACCESSOR(View, DXSharedResource<ID3D11ShaderResourceView>, _srv);
			GET_ACCESSOR(Raw, ID3D11ShaderResourceView*, _srv.GetRaw());

			template <class DXResourceType>
			void InitializeUsingTexture(Device::DirectX& dx, DXResourceType& resource, DXGI_FORMAT format, uint mipLevel, D3D11_SRV_DIMENSION viewDimension)
			{
				InitializeUsingTexture(dx, resource.GetRaw(), format, mipLevel, viewDimension);
			}
			void InitializeUsingBuffer(Device::DirectX& dx, DXSharedResource<ID3D11Buffer>& buffer, uint num, DXGI_FORMAT format, bool isRawBuffer);
			void InitializeUsingBuffer(Device::DirectX& dx, const Buffer::BaseBuffer& buffer, uint num, DXGI_FORMAT format, bool isRawBuffer);

		private:
			void InitializeUsingTexture(Device::DirectX& dx, ID3D11Resource* resource, DXGI_FORMAT format, uint mipLevel, D3D11_SRV_DIMENSION viewDimension);

		private:
			DXSharedResource<ID3D11ShaderResourceView>		_srv;
		};
	}
}
