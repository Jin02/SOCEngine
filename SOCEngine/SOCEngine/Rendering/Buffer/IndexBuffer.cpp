#include "IndexBuffer.h"
#include "Director.h"

using namespace Rendering::Buffer;
using namespace Device;

IndexBuffer::IndexBuffer() : BaseBuffer(), _indexCount(0)
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::Initialize(
	const std::vector<uint>& indices, const std::string& useVertexBufferKey, bool isDynamic)
{
	_indexCount = indices.size();
	_useVertexBufferKey = useVertexBufferKey;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(ENGINE_INDEX_TYPE) * _indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indices.data();

	ID3D11Device* device = Director::SharedInstance()->GetDirectX()->GetDevice();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &_buffer);

	ASSERT_COND_MSG(SUCCEEDED(hr), "Error!. does not create ib");

	return true;
}

void IndexBuffer::IASetBuffer(ID3D11DeviceContext* context)
{
	context->IASetIndexBuffer(_buffer, DXGI_FORMAT_R32_UINT, 0);
}