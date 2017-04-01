#include "VertexShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering;

VertexShader::VertexShader(const DXResource<ID3DBlob>& blob, const std::string& key)
	: _baseShader(blob, key), _shader(nullptr), _layout(nullptr)
{
}

void VertexShader::Initialize(
	Device::DirectX& dx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations)
{
	_shader	= dx.CreateVertexShader(_baseShader);
	_layout	= dx.CreateInputLayout(_baseShader, vertexDeclations);

	uint count = vertexDeclations.size();
	for(uint i=0; i<count; ++i)
	{
		const D3D11_INPUT_ELEMENT_DESC& desc = vertexDeclations[i];

		SemanticInfo info;
		{
			info.name = desc.SemanticName;
			info.semanticIndex = desc.SemanticIndex;

			if ((i + 1) != count)
				info.size = vertexDeclations[i + 1].AlignedByteOffset - desc.AlignedByteOffset;
		}

		_semanticInfo.push_back(info);
	}

	_semanticInfo.back().size = dx.CalcFormatSize(vertexDeclations[count-1].Format);
}

void VertexShader::BindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->VSSetShader(_shader.GetRaw(), nullptr, 0);
}

void VertexShader::BindInputLayoutToContext(Device::DirectX& dx)
{
	dx.GetContext()->IASetInputLayout(_layout.GetRaw());
}

void VertexShader::UnBindShaderToContext(Device::DirectX& dx)
{
	dx.GetContext()->VSSetShader(nullptr, nullptr, 0);
}

void VertexShader::UnBindInputLayoutToContext(Device::DirectX& dx)
{
	dx.GetContext()->IASetInputLayout(nullptr);
}

void VertexShader::BindTexture(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Texture::BaseTexture> tex)
{
	ID3D11ShaderResourceView* srv = (tex.expired() == false) ? tex.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->VSSetShaderResources(uint(bind), 1, &srv);
}

void VertexShader::BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, const std::weak_ptr<ID3D11SamplerState> samplerState)
{
	ID3D11SamplerState* sampler = (samplerState.expired() == false) ? samplerState.lock().get() : nullptr;
	dx.GetContext()->VSSetSamplers(uint(bind), 1, &sampler);
}

void VertexShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, const std::weak_ptr<Buffer::ConstBuffer> cb)
{
	ID3D11Buffer* buf = (cb.expired() == false) ? cb.lock().get()->GetBaseBuffer().GetBuffer().GetRaw() : nullptr;
	dx.GetContext()->VSSetConstantBuffers(uint(bind), 1, &buf);
}

void VertexShader::BindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind, const std::weak_ptr<Buffer::ShaderResourceBuffer> srBuffer)
{
	ID3D11ShaderResourceView* srv = (srBuffer.expired() == false) ? srBuffer.lock().get()->GetShaderResourceView().GetView().GetRaw() : nullptr;
	dx.GetContext()->VSSetShaderResources(uint(bind), 1, &srv);
}

void VertexShader::UnBindTexture(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->VSSetShaderResources(uint(bind), 1, &srv);
}

void VertexShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->VSSetSamplers(uint(bind), 1, &sampler);
}

void VertexShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->VSSetConstantBuffers(uint(bind), 1, &buf);
}

void VertexShader::UnBindShaderResourceBuffer(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->VSSetShaderResources(uint(bind), 1, &srv);
}