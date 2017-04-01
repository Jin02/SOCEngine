#include "ComputeShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering;

ComputeShader::ComputeShader(const DXResource<ID3DBlob>& blob, const std::string& key)
	: _base(blob, key), _shader(nullptr), _threadGroup(0, 0, 0)
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

void ComputeShader::BindTexture(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Texture::BaseTexture> tex)
{	
	ID3D11ShaderResourceView* srv = (tex.expired() == false) ? tex.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void ComputeShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, const std::weak_ptr<ID3D11SamplerState> samplerState)
{
	ID3D11SamplerState* sampler= (samplerState.expired() == false) ? samplerState.lock().get() : nullptr;
	dx.GetContext()->CSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void ComputeShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const std::weak_ptr<Buffer::ConstBuffer> cb)
{
	ID3D11Buffer* buf = (cb.expired() == false) ? cb.lock().get()->GetBaseBuffer().GetBuffer().GetRaw() : nullptr;
	dx.GetContext()->CSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

void ComputeShader::BindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Buffer::ShaderResourceBuffer> srBuffer)
{
	ID3D11ShaderResourceView* srv = (srBuffer.expired() == false) ? srBuffer.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void ComputeShader::BindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind, const std::weak_ptr<View::UnorderedAccessView> uav, const uint* initialCounts)
{
	ID3D11UnorderedAccessView* view = (uav.expired() == false) ? uav.lock().get()->GetView().GetRaw() : nullptr;
	dx.GetContext()->CSSetUnorderedAccessViews(static_cast<uint>(bind), 1, &view, initialCounts);
}

void ComputeShader::UnBindTexture(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &srv);
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

void ComputeShader::UnBindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->CSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void ComputeShader::UnBindUnorderedAccessView(Device::DirectX& dx, UAVBindIndex bind)
{
	ID3D11UnorderedAccessView* view = nullptr;
	dx.GetContext()->CSSetUnorderedAccessViews(static_cast<uint>(bind), 1, &view, nullptr);
}