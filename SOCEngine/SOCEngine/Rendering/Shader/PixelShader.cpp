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

void PixelShader::BindTexture(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Texture::BaseTexture> tex)
{
	ID3D11ShaderResourceView* srv = (tex.expired() == false) ? tex.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->PSSetShaderResources(uint(bind), 1, &srv);
}

void PixelShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, const std::weak_ptr<ID3D11SamplerState> samplerState)
{
	ID3D11SamplerState* sampler = (samplerState.expired() == false) ? samplerState.lock().get() : nullptr;
	dx.GetContext()->PSSetSamplers(uint(bind), 1, &sampler);
}

void PixelShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const std::weak_ptr<Buffer::ConstBuffer> cb)
{
	ID3D11Buffer* buf = (cb.expired() == false) ? cb.lock().get()->GetBaseBuffer().GetBuffer().GetRaw() : nullptr;
	dx.GetContext()->PSSetConstantBuffers(uint(bind), 1, &buf);
}

void PixelShader::BindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Buffer::ShaderResourceBuffer> srBuffer)
{
	ID3D11ShaderResourceView* srv = (srBuffer.expired() == false) ? srBuffer.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->PSSetShaderResources(uint(bind), 1, &srv);
}

void PixelShader::UnBindTexture(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->PSSetShaderResources(uint(bind), 1, &srv);
}

void PixelShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->PSSetSamplers(uint(bind), 1, &sampler);
}

void PixelShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->PSSetConstantBuffers(uint(bind), 1, &buf);
}

void PixelShader::UnBindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->PSSetShaderResources(uint(bind), 1, &srv);
}