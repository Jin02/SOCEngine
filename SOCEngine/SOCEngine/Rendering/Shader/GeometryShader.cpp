#include "GeometryShader.h"

using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering;

GeometryShader::GeometryShader(const DXResource<ID3DBlob>& blob, const std::string& key)
	: _shader(nullptr), _baseShader(blob, key)
{
}

void GeometryShader::Initialize(Device::DirectX& dx)
{
	_shader = dx.CreateGeometryShader(_baseShader);
}

void GeometryShader::BindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->GSSetShader(_shader.GetRaw(), nullptr, 0);
}

void GeometryShader::UnBindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->GSSetShader(nullptr, nullptr, 0);
}

void GeometryShader::BindTexture(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Texture::BaseTexture> tex)
{
	ID3D11ShaderResourceView* srv = (tex.expired() == false) ? tex.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->GSSetShaderResources(uint(bind), 1, &srv);
}

void GeometryShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, const std::weak_ptr<ID3D11SamplerState> samplerState)
{	
	ID3D11SamplerState* sampler = (samplerState.expired() == false) ? samplerState.lock().get() : nullptr;
	dx.GetContext()->GSSetSamplers(uint(bind), 1, &sampler);
}

void GeometryShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const std::weak_ptr<Buffer::ConstBuffer> cb)
{
	ID3D11Buffer* buf = (cb.expired() == false) ? cb.lock().get()->GetBaseBuffer().GetBuffer().GetRaw() : nullptr;
	dx.GetContext()->GSSetConstantBuffers(uint(bind), 1, &buf);
}

void GeometryShader::BindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Buffer::ShaderResourceBuffer> srBuffer)
{
	ID3D11ShaderResourceView* srv = (srBuffer.expired() == false) ? srBuffer.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->GSSetShaderResources(uint(bind), 1, &srv);
}

void GeometryShader::UnBindTexture(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->GSSetShaderResources(uint(bind), 1, &srv);
}

void GeometryShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->GSSetSamplers(uint(bind), 1, &sampler);
}

void GeometryShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->GSSetConstantBuffers(uint(bind), 1, &buf);
}

void GeometryShader::UnBindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->GSSetShaderResources(uint(bind), 1, &srv);
}