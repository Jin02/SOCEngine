#include "PixelShader.h"

using namespace Rendering;
using namespace Rendering::Shader;

PixelShader::PixelShader(const DXResource<ID3DBlob>& blob, const std::string& key)
	: _baseShader(blob, key), _shader(nullptr)
{
}

void PixelShader::Initialize(Device::DirectX& dx)
{
	_shader = dx.CreatePixelShader(_baseShader);
}

void PixelShader::BindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->PSSetShader(_shader.GetRaw(), nullptr, 0);
}

void PixelShader::UnBindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->PSSetShader(nullptr, nullptr, 0);
}


void PixelShader::BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, const View::ShaderResourceView& view)
{
	ID3D11ShaderResourceView* srv = view.GetView().GetRaw();
	dx.GetContext()->PSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void PixelShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, DXResource<ID3D11SamplerState>& samplerState)
{
	ID3D11SamplerState* sampler = samplerState.GetRaw();
	dx.GetContext()->PSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void PixelShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const Buffer::ConstBuffer& cb)
{
	ID3D11Buffer* buf = cb.GetBaseBuffer().GetBuffer().GetRaw();
	dx.GetContext()->PSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}


void PixelShader::UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->PSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void PixelShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->PSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void PixelShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->PSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}
