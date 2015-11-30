#include "UnorderedAccessView.h"
#include "Director.h"

using namespace Rendering::View;

UnorderedAccessView::UnorderedAccessView(ID3D11UnorderedAccessView* uav) : _uav(uav)
{
}

UnorderedAccessView::~UnorderedAccessView()
{
	Destroy();
}

void UnorderedAccessView::Initialize(
	DXGI_FORMAT format, uint numElements, ID3D11Resource* resource,
	D3D11_UAV_DIMENSION viewDimension,
	uint tex3dMipSlice, uint tex3dWSize)
{
	ASSERT_COND_MSG(_uav == nullptr, "Error, _uav has already been allocated");

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	desc.Format = format;
	desc.ViewDimension = viewDimension;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = numElements;

	if(viewDimension == D3D11_UAV_DIMENSION_TEXTURE3D)
	{
		desc.Texture3D.FirstWSlice	= 0;
		desc.Texture3D.MipSlice		= tex3dMipSlice;
		desc.Texture3D.WSize		= tex3dWSize;
	}

	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	HRESULT hr = device->CreateUnorderedAccessView(resource, &desc, &_uav);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, Cant create UAV");
}

void UnorderedAccessView::Destroy()
{
	SAFE_RELEASE(_uav);
}