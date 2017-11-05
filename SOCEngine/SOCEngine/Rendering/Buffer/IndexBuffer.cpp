#include "IndexBuffer.h"
#include "DirectX.h"

using namespace Rendering::Buffer;
using namespace Device;

void IndexBuffer::Initialize(Device::DirectX& dx, const std::vector<uint>& indices, BaseBuffer::Key vbKey, bool isDynamic)
{
	_indexCount	= indices.size();
	_vbKey		= vbKey;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage				= isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth			= sizeof(ENGINE_INDEX_TYPE) * _indexCount;
	bufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags		= 0;
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indices.data();

	_baseBuffer.SetBuffer(dx.CreateBuffer(bufferDesc, &data));
}

void IndexBuffer::IASetBuffer(Device::DirectX& dx)
{
	dx.GetContext()->IASetIndexBuffer(_baseBuffer.GetRaw(), DXGI_FORMAT_R32_UINT, 0);
}
