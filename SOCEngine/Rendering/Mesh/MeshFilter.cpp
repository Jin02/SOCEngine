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

bool MeshFilter::CreateBuffer(const std::vector<const void*>& vbDatas, unsigned int vertexBufferSize, const std::vector<ENGINE_INDEX_TYPE>& indices, bool isDynamic)
{
	if(vbDatas.size() == 0)
		return false;

	_numOfVertex = vbDatas.size();

	_vertexBuffer = new Buffer::VertexBuffer;
	if( _vertexBuffer->Create(vbDatas.data(), vertexBufferSize, _numOfVertex, isDynamic) == false )
		return false;

	_indexBuffer = new Buffer::IndexBuffer;
	if( _indexBuffer->Create(indices.data(), sizeof(ENGINE_INDEX_TYPE) * indices.size()) == false )
		return false;

	return true;
}

void MeshFilter::UpdateBuffer(ID3D11DeviceContext* context)
{
	_vertexBuffer->UpdateBuffer(context);
	_indexBuffer->UpdateBuffer(context);
}
