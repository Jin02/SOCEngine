#include "Sampler.h"
#include "Director.h"

using namespace Rendering;

Sampler::Sampler() : _sampler(nullptr)
{

}

Sampler::~Sampler()
{
	Destroy();
}

void Sampler::Create()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11Device* device = Device::Director::GetInstance()->GetDirectX()->GetDevice();
	HRESULT hr  = device->CreateSamplerState( &sampDesc, &_sampler );
}

void Sampler::Destroy()
{
	SAFE_RELEASE(_sampler);
}