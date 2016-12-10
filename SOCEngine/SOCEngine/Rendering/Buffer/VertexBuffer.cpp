#include "VertexBuffer.h"
#include "Director.h"

using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Device;

VertexBuffer::VertexBuffer() : BaseBuffer(), _key("")
{
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::Initialize(
	const void* sysMem, unsigned int bufferStrideSize, unsigned int count,
	bool isDynamic, const std::string& key,
	const std::vector<VertexShader::SemanticInfo>* semanticInfos)
{
	if(semanticInfos)
		_semantics = (*semanticInfos);

	_stride			= bufferStrideSize;
	_vertexCount	= count;
	_key			= key;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = bufferStrideSize * count;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = sysMem;

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &_buffer);

	ASSERT_MSG_IF(SUCCEEDED(hr), "Error!. does not create vb");
}

void VertexBuffer::IASetBuffer(ID3D11DeviceContext* context)
{
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &_buffer, &_stride, &offset); 
}

void VertexBuffer::UpdateVertexData(ID3D11DeviceContext* context, const void* data, uint size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ASSERT_MSG_IF( SUCCEEDED(hr), "Error, VertexBuffer cant execute dx Map func");

	memcpy(mappedResource.pData, data, size);

	context->Unmap(_buffer, 0);
}