#include "ComputeShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::RenderState;

ComputeShader::ComputeShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key) : _base(blob, key)
{

}

void ComputeShader::Initialize(DirectX& dx)
{
	_shader = dx.CreateComputeShader(_base);
}

void ComputeShader::Dispatch(Device::DirectX& dx, const ComputeShader::ThreadGroup& group)
{
	dx.GetContext()->CSSetShader(_shader.GetRaw(), nullptr, 0);
	dx.GetContext()->Dispatch(group.x, group.y, group.z);
	dx.GetContext()->CSSetShader(nullptr, nullptr, 0);
}

void ComputeShader::BindShaderResourceView(DirectX& dx, TextureBindIndex bind, const ShaderResourceView& srv)
{	
	ID3D11ShaderResourceView* raw = const_cast<ShaderResourceView&>(srv).GetRaw();
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &raw);
}

void ComputeShader::BindSamplerState(DirectX& dx, SamplerStateBindIndex bind, SamplerState state)
{	
	auto samplerState = const_cast<ID3D11SamplerState*>( dx.GetSamplerState(state).GetRaw() );
	dx.GetContext()->CSSetSamplers(static_cast<uint>(bind), 1, &samplerState);
}

void ComputeShader::BindConstBuffer(DirectX& dx, ConstBufferBindIndex bind, const ConstBuffer& cb)
{
	ID3D11Buffer* buf = const_cast<BaseBuffer&>(cb.GetBaseBuffer()).GetRaw();
	dx.GetContext()->CSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void ComputeShader::BindUnorderedAccessView(DirectX& dx, UAVBindIndex bind, const UnorderedAccessView& uav, const uint* initialCounts)
{
	ID3D11UnorderedAccessView* view = const_cast<UnorderedAccessView&>(uav).GetRaw();
	dx.GetContext()->CSSetUnorderedAccessViews(static_cast<uint>(bind), 1, &view, initialCounts);
}

void ComputeShader::UnBindSamplerState(DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->CSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void ComputeShader::UnBindConstBuffer(DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->CSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void ComputeShader::UnBindShaderResourceView(DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void ComputeShader::UnBindUnorderedAccessView(DirectX& dx, UAVBindIndex bind)
{
	ID3D11UnorderedAccessView* view = nullptr;
	dx.GetContext()->CSSetUnorderedAccessViews(static_cast<uint>(bind), 1, &view, nullptr);
}
