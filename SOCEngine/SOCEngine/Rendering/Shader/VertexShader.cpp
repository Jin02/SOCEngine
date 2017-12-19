#include "VertexShader.h"

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::RenderState;

VertexShader::VertexShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key)
	: _baseShader(blob, key), _shader(nullptr), _layout(nullptr)
{
}

void VertexShader::Initialize(DirectX& dx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations)
{
	_shader	= dx.CreateVertexShader(_baseShader);

	if(vertexDeclations.empty()) return;
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

void VertexShader::Destroy()
{
	_shader.Destroy();
	_baseShader.Destroy();
	_layout.Destroy();
	
	_semanticInfo.clear();
}

void VertexShader::BindShaderToContext(DirectX& dx) const
{
	dx.GetContext()->VSSetShader(const_cast<ID3D11VertexShader*>(_shader.GetRaw()), nullptr, 0);
}

void VertexShader::BindInputLayoutToContext(DirectX& dx) const
{
	dx.GetContext()->IASetInputLayout(const_cast<ID3D11InputLayout*>(_layout.GetRaw()));
}

void VertexShader::UnBindShaderToContext(DirectX& dx)
{
	dx.GetContext()->VSSetShader(nullptr, nullptr, 0);
}

void VertexShader::UnBindInputLayoutToContext(DirectX& dx)
{
	dx.GetContext()->IASetInputLayout(nullptr);
}

void VertexShader::BindShaderResourceView(DirectX& dx, TextureBindIndex bind, const ShaderResourceView& view)
{
	ID3D11ShaderResourceView* srv = const_cast<ShaderResourceView&>(view).GetRaw();
	dx.GetContext()->VSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void VertexShader::BindSamplerState(DirectX& dx, SamplerStateBindIndex bind, SamplerState state)
{
	auto samplerState = const_cast<ID3D11SamplerState*>( dx.GetSamplerState(state).GetRaw() );
	dx.GetContext()->VSSetSamplers(static_cast<uint>(bind), 1, const_cast<ID3D11SamplerState* const*>(&samplerState));
}

void VertexShader::BindConstBuffer(DirectX& dx, ConstBufferBindIndex bind, const ConstBuffer& cb)
{
	ID3D11Buffer* buf = const_cast<BaseBuffer&>(cb.GetBaseBuffer()).GetRaw();
	dx.GetContext()->VSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}

//void VertexShader::BindConstBuffer(DirectX& dx, ConstBufferBindIndex bind, OnlyAccess<ConstBuffer>&& cb)
//{
//	ID3D11Buffer* buf = cb.GetData().GetBaseBuffer().GetRaw();
//	dx.GetContext()->VSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
//}

void VertexShader::UnBindShaderResourceView(DirectX& dx, TextureBindIndex bind)
{
	ID3D11ShaderResourceView* srv = nullptr;
	dx.GetContext()->VSSetShaderResources(static_cast<uint>(bind), 1, &srv);
}

void VertexShader::UnBindSamplerState(DirectX& dx, SamplerStateBindIndex bind)
{
	ID3D11SamplerState* sampler = nullptr;
	dx.GetContext()->VSSetSamplers(static_cast<uint>(bind), 1, &sampler);
}

void VertexShader::UnBindConstBuffer(DirectX& dx, ConstBufferBindIndex bind)
{
	ID3D11Buffer* buf = nullptr;
	dx.GetContext()->VSSetConstantBuffers(static_cast<uint>(bind), 1, &buf);
}
