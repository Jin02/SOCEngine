#include "CSOutputBuffer_ReadCPU.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

CSOutputBuffer_ReadCPU::CSOutputBuffer_ReadCPU() : CSOutputBuffer()
{

}

CSOutputBuffer_ReadCPU::~CSOutputBuffer_ReadCPU()
{

}

bool CSOutputBuffer_ReadCPU::Create(unsigned int stride, unsigned int num, bool useShader)
{
	D3D11_BUFFER_DESC desc;
    desc.ByteWidth = stride * num;
	desc.StructureByteStride = stride;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | (useShader ? D3D11_BIND_SHADER_RESOURCE : 0u);
	desc.CPUAccessFlags = 0;

	ID3D11Device* device = Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, nullptr, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. does not create constant buffer");

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = num;

	hr = device->CreateUnorderedAccessView(_buffer, &uavDesc, &_uav);
	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!, does not create unordered acess view");

	return true;
}