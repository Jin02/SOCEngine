#include "VertexBuffer.h"

using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Device;

void VertexBuffer::Initialize(Device::DirectX& dx, const Desc& desc, const void* sysMem, bool isDynamic, const std::vector<VertexShader::SemanticInfo>& semanticInfos)
{
	_semantics	= semanticInfos;
	_desc		= desc;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = desc.stride * desc.vertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = sysMem;

	_baseBuffer.SetBuffer( dx.CreateBuffer(bufferDesc, &data) );
}

void VertexBuffer::IASetBuffer(Device::DirectX& dx)
{
	uint offset = 0;
	ID3D11Buffer* buffer = _baseBuffer.GetBuffer().GetRaw();
	dx.GetContext()->IASetVertexBuffers(0, 1, &buffer, &_desc.stride, &offset);
}

void VertexBuffer::UpdateVertexData(Device::DirectX& dx, const void* data, uint size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ASSERT_SUCCEEDED(dx.GetContext()->Map(_baseBuffer.GetBuffer().GetRaw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	memcpy(mappedResource.pData, data, size);

	dx.GetContext()->Unmap(_baseBuffer.GetBuffer().GetRaw(), 0);
}
