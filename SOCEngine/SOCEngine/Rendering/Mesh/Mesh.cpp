#include "Mesh.h"
#include "DefaultRenderTypes.h"
#include "DefaultShaderLoader.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Buffer;
using namespace Core;


void Mesh::Initialize(Device::DirectX& dx, BufferManager& bufferMgr, const CreateFuncArguments & args)
{
	uint vertexCount	= args.vertices.count;
	uint indexCount		= args.indices.size();

	std::string vbKey = args.fileName + ":" + args.key;

	if (bufferMgr.GetPool<VertexBuffer>().Has(args.fileName, args.key) == false)
	{
		VertexBuffer::Param param;
		{
			param.stride		= args.vertices.byteWidth;
			param.vertexCount	= args.vertices.count;
			param.key			= vbKey;
		}

		_vertexBuffer.Initialize(dx, param, args.vertices.data, args.useDynamicVB, args.semanticInfos);
		bufferMgr.GetPool<VertexBuffer>().Add(args.fileName, args.key, _vertexBuffer);
	}

	if (bufferMgr.GetPool<IndexBuffer>().Has(args.fileName, args.key) == false)
	{
		_indexBuffer.Initialize(dx, args.indices, vbKey, args.useDynamicIB);
		bufferMgr.GetPool<IndexBuffer>().Add(args.fileName, args.key, _indexBuffer);
	}	

	_bufferFlag = ComputeBufferFlag(args.semanticInfos);

	_tfCB.Initialize(dx);
}

void Mesh::Initialize(const Buffer::VertexBuffer & vertexBuffer, const Buffer::IndexBuffer & indexBuffer)
{
	_vertexBuffer	= vertexBuffer;
	_indexBuffer	= indexBuffer;

	_bufferFlag = ComputeBufferFlag(vertexBuffer.GetSemantics());
}

uint Mesh::ComputeBufferFlag(const std::vector<VertexShader::SemanticInfo>& semantics, uint maxRecognizeBoneCount) const
{
	uint flag = 0;
	for (auto iter = semantics.begin(); iter != semantics.end(); ++iter)
	{
		const auto& semantic = *iter;

		if (semantic.name == "POSITION")		continue;
		else if (semantic.name == "TEXCOORD")
		{
			if (semantic.semanticIndex > 1)
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::USERS);
			else
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::UV0) << semantic.semanticIndex;
		}
		else if (semantic.name == "NORMAL")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::NORMAL);
		else if (semantic.name == "TANGENT")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::TANGENT);
		else if (semantic.name == "COLOR")
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::COLOR);
		else if (semantic.name == "BONEWEIGHT")
		{
			if (semantic.semanticIndex + 1 >= maxRecognizeBoneCount)
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::BONE) << semantic.semanticIndex;
			else
				flag |= static_cast<uint>(DefaultVertexInputTypeFlag::USERS);
		}
		else
		{
			flag |= static_cast<uint>(DefaultVertexInputTypeFlag::USERS);
		}
	}

	// Error, You use undefined semantic in RenderManager::DefaultVertexInputTypeFlag.
	assert((flag & static_cast<uint>(DefaultVertexInputTypeFlag::USERS)) == 0);

	return flag;
}

void Mesh::AddMaterialKey(const std::string & key)
{
	_materialKeys.push_back(key);
}

bool Mesh::HasMaterialKey(const std::string & key) const
{
	for (auto& iter : _materialKeys)
	{
		if (iter == key)
			return true;
	}

	return false;
}

void Mesh::DeleteMaterialKey(const std::string & key)
{
	uint size = _materialKeys.size();
	for (uint i = 0; i < size; ++i)
	{
		if (_materialKeys[i] == key)
		{
			_materialKeys.erase( _materialKeys.begin() + i );
			return;
		}
	}
}
