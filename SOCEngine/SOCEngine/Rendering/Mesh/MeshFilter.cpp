#include "MeshFilter.h"
#include "Director.h"

using namespace Rendering::Mesh;

MeshFilter::MeshFilter() 
	:	_vertexBuffer(nullptr), _indexBuffer(nullptr), _flag(0),
		_alloc(false), _vertexCount(0), _indexCount(0)
{
}

MeshFilter::~MeshFilter()
{
	if(_alloc == false)
		return;

	SAFE_DELETE(_vertexBuffer);
	SAFE_DELETE(_indexBuffer);
}

bool MeshFilter::CreateBuffer(const CreateFuncArguments& args)
{
	_vertexCount	= args.vertex.count;
	_indexCount		= args.index.count;

	const Core::Scene* scene = Device::Director::GetInstance()->GetCurrentScene();
	auto bufferMgr = scene->GetBufferManager();

	// Vertex Buffer Setting
	{
		Buffer::VertexBuffer* vertexBuffer	= nullptr;
		if( bufferMgr->Find(&vertexBuffer, args.fileName, args.key) == false )
		{
			vertexBuffer = new Buffer::VertexBuffer;
			if( vertexBuffer->Initialize(args.vertex.data, args.vertex.byteWidth, _vertexCount, args.isDynamic) == false )
				ASSERT_MSG("Error, can not create vertex buffer");

			bufferMgr->Add(args.fileName, args.key, vertexBuffer);
		}

		_vertexBuffer = vertexBuffer;
	}

	// Index Buffer Setting
	{
		Buffer::IndexBuffer* indexBuffer	= nullptr;
		if( bufferMgr->Find(&indexBuffer, args.fileName, args.key) == false )
		{
			indexBuffer = new Buffer::IndexBuffer;
			if( indexBuffer->Initialize(args.index.data, sizeof(ENGINE_INDEX_TYPE) * _indexCount) == false )
				ASSERT_MSG("Error, can not create index buffer");

			bufferMgr->Add(args.fileName, args.key, indexBuffer);
		}

		_indexBuffer = indexBuffer;
	}

	_flag = args.bufferFlag;
	return true;
}

void MeshFilter::IASetBuffer(ID3D11DeviceContext* context)
{
	_vertexBuffer->IASetBuffer(context);
	_indexBuffer->IASetBuffer(context);
}

void MeshFilter::UpdateVertexBufferData(ID3D11DeviceContext* context, const void* data, uint size)
{
	_vertexBuffer->UpdateVertexData(context, data, size);
}