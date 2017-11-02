#include "VertexShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering;

VertexShader::VertexShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key)
	: _baseShader(blob, key), _shader(nullptr), _layout(nullptr)
{
}

void VertexShader::Initialize(Device::DirectX& dx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations)
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

void VertexShader::BindShaderResourceView(Device::DirectX& dx,
	TextureBindIndex bind, View::ShaderResourceView& view)
{
	ID3D11ShaderResourceView* srv = view.GetRaw();
	dx.GetContext()->VSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void VertexShader::BindSamplerState(Device::DirectX& dx,
									SamplerStateBindIndex bind, ID3D11SamplerState* const samplerState)
{
	dx.GetContext()->VSSetSamplers(static_cast<uint>(bind), 1, &samplerState);
}

void VertexShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, Buffer::ConstBuffer& cb)
{
	ID3D11Buffer* buf = cb.GetBaseBuffer().GetRaw();
	dx.GetContext()->VSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

//void VertexShader::BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, OnlyAccess<Buffer::ConstBuffer>&& cb)
//{
//	ID3D11Buffer* buf = cb.GetData().GetBaseBuffer().GetRaw();
//	dx.GetContext()->VSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
//}

void VertexShader::UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->VSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void VertexShader::UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->VSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void VertexShader::UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->VSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}
