#include "PixelShader.h"

using namespace Rendering;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::RenderState;
using namespace Device;

PixelShader::PixelShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key)
	: _baseShader(blob, key), _shader(nullptr)
{
}

void PixelShader::Initialize(DirectX& dx)
{
	_shader = dx.CreatePixelShader(_baseShader);
}

void PixelShader::BindShaderToContext(DirectX& dx) const
{
	dx.GetContext()->PSSetShader(const_cast<ID3D11PixelShader*>(_shader.GetRaw()), nullptr, 0);
}

void PixelShader::UnBindShaderToContext(DirectX& dx)
{
	dx.GetContext()->PSSetShader(nullptr, nullptr, 0);
}


void PixelShader::BindShaderResourceView(DirectX& dx, TextureBindIndex bind, const ShaderResourceView& view)
{
	ID3D11ShaderResourceView* srv = const_cast<ShaderResourceView&>(view).GetRaw();
	dx.GetContext()->PSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void PixelShader::BindSamplerState(DirectX& dx, SamplerStateBindIndex bind, SamplerState state)
{
	auto samplerState = const_cast<ID3D11SamplerState*>( dx.GetSamplerState(state).GetRaw() );
	dx.GetContext()->PSSetSamplers(static_cast<uint>(bind), 1, const_cast<ID3D11SamplerState* const*>(&samplerState));
}

void PixelShader::BindConstBuffer(DirectX& dx, ConstBufferBindIndex bind, const ConstBuffer& cb)
{
	ID3D11Buffer* buf = const_cast<ConstBuffer&>(cb).GetBaseBuffer().GetRaw();
	dx.GetContext()->PSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}


void PixelShader::UnBindShaderResourceView(DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->PSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void PixelShader::UnBindSamplerState(DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->PSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void PixelShader::UnBindConstBuffer(DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->PSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}
