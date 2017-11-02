#include "ComputeShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering;

ComputeShader::ComputeShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key) : _base(blob, key)
{

}

void ComputeShader::Initialize(Device::DirectX& dx)
{
	_shader = dx.CreateComputeShader(_base);
}

void ComputeShader::Dispatch(Device::DirectX& dx)
{
	dx.GetContext()->CSSetShader(_shader.GetRaw(), nullptr, 0);
	dx.GetContext()->Dispatch(_threadGroup.x, _threadGroup.y, _threadGroup.z);
	dx.GetContext()->CSSetShader(nullptr, nullptr, 0);
}

void ComputeShader::BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, View::ShaderResourceView& srv)
{	
	ID3D11ShaderResourceView* raw = srv.GetRaw();
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &raw);
}

void ComputeShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, ID3D11SamplerState* samplerState)
{
	dx.GetContext()->CSSetSamplers(static_cast<uint>(bind), 1, &samplerState);
}

void ComputeShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, Buffer::ConstBuffer& cb)
{
	ID3D11Buffer* buf = cb.GetBaseBuffer().GetRaw();
	dx.GetContext()->CSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void ComputeShader::BindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind, View::UnorderedAccessView& uav, const uint* initialCounts)
{
	ID3D11UnorderedAccessView* view = uav.GetRaw();
	dx.GetContext()->CSSetUnorderedAccessViews(static_cast<uint>(bind), 1, &view, initialCounts);
}

void ComputeShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->CSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void ComputeShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->CSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void ComputeShader::UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void ComputeShader::UnBindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind)
{
	ID3D11UnorderedAccessView* view = nullptr;
	dx.GetContext()->CSSetUnorderedAccessViews(static_cast<uint>(bind), 1, &view, nullptr);
}
