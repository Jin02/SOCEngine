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

void CSOutput::Initialize(DXGI_FORMAT format, uint numElements, ID3D11Resource* resource)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	desc.Format = format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D; //uav는 multisampler를 제공하지 않음
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