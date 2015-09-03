#include "CSOutput.h"
#include "Director.h"

using namespace GPGPU::DirectCompute;

CSOutput::CSOutput() : _uav(nullptr)
{
}

CSOutput::~CSOutput()
{
	Destroy();
}

void CSOutput::Initialize(DXGI_FORMAT format, uint numElements, ID3D11Resource* resource, Type type)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	desc.Format = format;

	if(type == Type::Buffer)
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	else if(type == Type::Texture)
		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = numElements;

	const Device::DirectX* dx = Device::Director::GetInstance()->GetDirectX();
	ID3D11Device* device = dx->GetDevice();

	HRESULT hr = device->CreateUnorderedAccessView(resource, &desc, &_uav);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error, Cant create UAV");
}

void CSOutput::Destroy()
{
	SAFE_RELEASE(_uav);
}