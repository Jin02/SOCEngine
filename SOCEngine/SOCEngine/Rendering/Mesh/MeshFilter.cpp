#include "MeshFilter.h"
#include "Director.h"
#include "ResourceManager.h"
#include "MeshImporter.h"

using namespace Rendering::Shader;
using namespace Rendering::Mesh;
using namespace Rendering::Manager;
using namespace Resource;

MeshFilter::MeshFilter() 
	:	_vertexBuffer(nullptr), _indexBuffer(nullptr), _alloc(false)
{
}

MeshFilter::~MeshFilter()
{
	if(_alloc == false)
		return;

	SAFE_DELETE(_vertexBuffer);
	SAFE_DELETE(_indexBuffer);
}

bool MeshFilter::Initialize(const CreateFuncArguments& args)
{
	ASSERT_COND_MSG(args.indices, "Error, Indices is null!");

	uint vertexCount	= args.vertices.count;
	uint indexCount		= args.indices->size();

	Manager::BufferManager* bufferMgr = ResourceManager::GetInstance()->GetBufferManager();

	std::string vbKey = args.fileName + ":" + args.key;

	// Vertex Buffer Setting
	{
		Buffer::VertexBuffer* vertexBuffer	= nullptr;
		if( bufferMgr->Find(&vertexBuffer, args.fileName, args.key) == false )
		{
			vertexBuffer = new Buffer::VertexBuffer;
			vertexBuffer->Initialize(args.vertices.data, args.vertices.byteWidth, vertexCount, args.useDynamicVB, vbKey, args.semanticInfos);
			
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
			if( indexBuffer->Initialize(*args.indices, vbKey, args.useDynamicIB) == false )
				ASSERT_MSG("Error, can not create index buffer");

			bufferMgr->Add(args.fileName, args.key, indexBuffer);
		}

		_indexBuffer = indexBuffer;
	}

	_bufferFlag = args.semanticInfos ? ComputeBufferFlag(*args.semanticInfos) : 0;

	return true;
}

bool MeshFilter::Initialize(Rendering::Buffer::VertexBuffer*& vertexBuffer, Rendering::Buffer::IndexBuffer*& indexBuffer)
{
	_vertexBuffer	= vertexBuffer;
	_indexBuffer	= indexBuffer;

	_bufferFlag		= ComputeBufferFlag(_vertexBuffer->GetSemantics());

	return true;
}

uint MeshFilter::ComputeBufferFlag(
	const std::vector<VertexShader::SemanticInfo>& semantics) const
{
	uint flag = 0;
	for(auto iter = semantics.begin(); iter != semantics.end(); ++iter)
	{
		const auto& semantic = *iter;

		if(		semantic.name == "POSITION")		continue;
		else if(semantic.name == "TEXCOORD")
		{
			if(semantic.semanticIndex > 1)
				flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::USERS;
			else
				flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::UV0 << semantic.semanticIndex;
		}
		else if(semantic.name == "NORMAL")
			flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::NORMAL;
		else if(semantic.name == "TANGENT")
			flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::TANGENT;
		else if(semantic.name == "COLOR")
			flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::COLOR;
		else if(semantic.name == "BONEWEIGHT")
		{
			if(semantic.semanticIndex+1 >= Importer::MeshImporter::MaximumRecognizeBoneCount)
				flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::BONE << semantic.semanticIndex;
			else
				flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::USERS;
		}
		else
		{
			flag |= (uint)RenderManager::DefaultVertexInputTypeFlag::USERS;
		}
	}

	if(flag & (uint)RenderManager::DefaultVertexInputTypeFlag::USERS)
	{
		DEBUG_LOG("Warning, You use undefined semantic in RenderManager::DefaultVertexInputTypeFlag.");
	}

	return flag;
}