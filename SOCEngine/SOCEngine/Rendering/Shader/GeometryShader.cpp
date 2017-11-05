#include "GeometryShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering;
using namespace Rendering::RenderState;

GeometryShader::GeometryShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key)
	: _shader(nullptr), _baseShader(blob, key)
{
}

void GeometryShader::Initialize(DirectX& dx)
{
	_shader = dx.CreateGeometryShader(_baseShader);
}

void GeometryShader::BindShaderToContext(DirectX& dx) const
{
	dx.GetContext()->GSSetShader(const_cast<ID3D11GeometryShader*>(_shader.GetRaw()), nullptr, 0);
}

void GeometryShader::UnBindShaderToContext(DirectX& dx)
{
	dx.GetContext()->GSSetShader(nullptr, nullptr, 0);
}

void GeometryShader::BindShaderResourceView(DirectX& dx, TextureBindIndex bind, const ShaderResourceView& srv)
{
	ID3D11ShaderResourceView* raw = const_cast<ShaderResourceView&>(srv).GetRaw();
	dx.GetContext()->GSSetShaderResources(static_cast<uint>(bind), 1, &raw);
}

void GeometryShader::BindSamplerState(DirectX& dx, SamplerStateBindIndex bind, SamplerState state)
{	
	auto samplerState = const_cast<ID3D11SamplerState*>( dx.GetSamplerState(state).GetRaw() );
	dx.GetContext()->GSSetSamplers(static_cast<uint>(bind), 1, const_cast<ID3D11SamplerState* const*>(&samplerState));
}

void GeometryShader::BindConstBuffer(DirectX& dx, ConstBufferBindIndex bind, const ConstBuffer& cb)
{
	ID3D11Buffer* buf = const_cast<ConstBuffer&>(cb).GetBaseBuffer().GetRaw();
	dx.GetContext()->GSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void GeometryShader::UnBindShaderResourceView(DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->GSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void GeometryShader::UnBindSamplerState(DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->GSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void GeometryShader::UnBindConstBuffer(DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->GSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}
