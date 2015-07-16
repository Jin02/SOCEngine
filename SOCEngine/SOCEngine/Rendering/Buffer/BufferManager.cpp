#include "BufferManager.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Manager;

BufferManager::BufferManager()
{
}

BufferManager::~BufferManager()
{
	DeleteAllIndexBuffer();
	DeleteAllVertexBuffer();
	DeleteAllOriginVBData();
}

void BufferManager::Add(const std::string& file, const std::string& key, VertexBuffer* bufferData)
{
	ASSERT_COND_MSG(_vertexBuffers.Add(file, key, bufferData), "Error, Duplicated Key");
}

void BufferManager::Add(const std::string& file, const std::string& key, IndexBuffer* bufferData)
{
	ASSERT_COND_MSG(_indexBuffers.Add(file, key, bufferData), "Error, Duplicated Key");
}

void BufferManager::Add(const std::string& file, const std::string& key, const LPVoidType& bufferData)
{
	_originVertexBufferDatas.Add(file, key, bufferData);
}

bool BufferManager::Find(VertexBuffer** outBuffer, const std::string& file, const std::string& key)
{
	VertexBuffer* buffer = *_vertexBuffers.Find(file, key);

	if(buffer != nullptr)
		(*outBuffer) = buffer;

	return buffer != nullptr;
}

bool BufferManager::Find(IndexBuffer** outBuffer, const std::string& file, const std::string& key)
{
	IndexBuffer* buffer = *_indexBuffers.Find(file, key);

	if(buffer != nullptr)
		(*outBuffer) = buffer;

	return buffer != nullptr;
}

bool BufferManager::Find(LPVoidType* outBuffer, const std::string& file, const std::string& key)
{
	LPVoidType* buffer = _originVertexBufferDatas.Find(file, key);

	if(buffer != nullptr)
		(*outBuffer) = (*buffer);

	return buffer != nullptr;
}

void BufferManager::DeleteVertexBuffer(const std::string& file, const std::string& key)
{
	VertexBuffer** vb = _vertexBuffers.Find(file, key);
	if(vb)
		SAFE_DELETE(*vb);

	_vertexBuffers.Delete(file, key);
}

void BufferManager::DeleteIndexBuffer(const std::string& file, const std::string& key)
{
	IndexBuffer** ib = _indexBuffers.Find(file, key);
	if(ib)
		SAFE_DELETE(*ib);

	_indexBuffers.Delete(file, key);
}

void BufferManager::DeleteOriginVBData(const std::string& file, const std::string& key)
{
	LPVoidType* ovb =  _originVertexBufferDatas.Find(file, key);
	if(ovb)
	{
		const void* buffer = ovb->GetBuffer();
		SAFE_DELETE(buffer);
	}

	_originVertexBufferDatas.Delete(file, key);
}

void BufferManager::DeleteVertexBuffer(const std::string& file)
{
	std::map<std::string, VertexBuffer*> vertexBuffers;
	if( _vertexBuffers.Find(vertexBuffers, file) )
	{
		for(auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			SAFE_DELETE(iter->second);

		_vertexBuffers.Delete(file);
	}
}

void BufferManager::DeleteIndexBuffer(const std::string& file)
{
	std::map<std::string, IndexBuffer*> indexBuffers;
	if( _indexBuffers.Find(indexBuffers, file) )
	{
		for(auto iter = indexBuffers.begin(); iter != indexBuffers.end(); ++iter)
			SAFE_DELETE(iter->second);

		_indexBuffers.Delete(file);
	}
}

void BufferManager::DeleteOriginVBData(const std::string& file)
{
	std::map<std::string, LPVoidType> ovbs;
	if( _originVertexBufferDatas.Find(ovbs, file) )
	{
		for(auto iter = ovbs.begin(); iter != ovbs.end(); ++iter)
		{
			const void* buffer = iter->second.GetBuffer();
			SAFE_DELETE(buffer);
		}

		_originVertexBufferDatas.Delete(file);
	}
}

void BufferManager::DeleteAllVertexBuffer()
{
	auto mim = _vertexBuffers.GetMapInMap();
	for(auto fileLoopIter = mim.begin(); fileLoopIter != mim.end(); ++fileLoopIter)
	{
		auto inMap = fileLoopIter->second;
		for(auto iter = inMap.begin(); iter != inMap.end(); ++iter)
		{
			SAFE_DELETE(iter->second);
		}
	}

	mim.clear();
}

void BufferManager::DeleteAllIndexBuffer()
{
	auto mim = _indexBuffers.GetMapInMap();
	for(auto fileLoopIter = mim.begin(); fileLoopIter != mim.end(); ++fileLoopIter)
	{
		auto inMap = fileLoopIter->second;
		for(auto iter = inMap.begin(); iter != inMap.end(); ++iter)
		{
			SAFE_DELETE(iter->second);
		}
	}

	mim.clear();
}

void BufferManager::DeleteAllOriginVBData()
{
	auto mim = _originVertexBufferDatas.GetMapInMap();
	for(auto fileLoopIter = mim.begin(); fileLoopIter != mim.end(); ++fileLoopIter)
	{
		auto inMap = fileLoopIter->second;
		for(auto iter = inMap.begin(); iter != inMap.end(); ++iter)
		{
			const void* buffer = iter->second.GetBuffer();
			SAFE_DELETE(buffer);
		}
	}

	mim.clear();
}
