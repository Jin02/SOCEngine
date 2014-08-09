#include "MeshFilter.h"

using namespace Rendering::Mesh;

MeshFilter::MeshFilter() : _vertexBuffer(nullptr), _indexBuffer(nullptr)
{
	_alloc = false;
	_numOfVertex = 0;
}

MeshFilter::~MeshFilter()
{
	if(_alloc == false)
		return;

	SAFE_DELETE(_vertexBuffer);
	SAFE_DELETE(_indexBuffer);
}

bool MeshFilter::CreateBuffer(const void* vertexBufferDatas, unsigned int vertexBufferDataCount, unsigned int vertexBufferSize,
				const ENGINE_INDEX_TYPE* indicesData, unsigned int indicesCount, bool isDynamic)
{
	_numOfVertex = vertexBufferDataCount;

	_vertexBuffer = new Buffer::VertexBuffer;
	if( _vertexBuffer->Create(vertexBufferDatas, vertexBufferSize, _numOfVertex, isDynamic) == false )
		return false;

	_indexBuffer = new Buffer::IndexBuffer;
	if( _indexBuffer->Create(indicesData, sizeof(ENGINE_INDEX_TYPE) * indicesCount) == false )
		return false;

	return true;
}

void MeshFilter::UpdateBuffer(ID3D11DeviceContext* context)
{
	_vertexBuffer->UpdateBuffer(context);
	_indexBuffer->UpdateBuffer(context);
}
