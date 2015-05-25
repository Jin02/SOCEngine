#include "BufferManager.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Manager;

BufferManager::BufferManager()
{
}

BufferManager::~BufferManager()
{
	DeleteAllIndexBuffer(true);
	DeleteAllVertexBuffer(true);
	DeleteAllOriginVBData(true);
}

void BufferManager::Add(const std::string file, const std::string key, VertexBuffer* bufferData, bool copy)
{
	_vertexBuffers.Add(file, key, bufferData, copy);
}

void BufferManager::Add(const std::string file, const std::string key, IndexBuffer* bufferData, bool copy)
{
	_indexBuffers.Add(file, key, bufferData, copy);
}

void BufferManager::Add(const std::string file, const std::string key, LPVoidType* bufferData, bool copy)
{
	_originVertexBufferDatas.Add(file, key, bufferData, copy);
}

bool BufferManager::Find(VertexBuffer** outBuffer, const std::string file, const std::string key)
{
	VertexBuffer* buffer = _vertexBuffers.Find(file, key);
	bool success = ( buffer != nullptr );

	if(outBuffer)
		(*outBuffer) = buffer;

	return success;
}

bool BufferManager::Find(IndexBuffer** outBuffer, const std::string file, const std::string key)
{
	IndexBuffer* buffer = _indexBuffers.Find(file, key);
	bool success = ( buffer != nullptr );

	if(outBuffer)
		(*outBuffer) = buffer;

	return success;
}

bool BufferManager::Find(LPVoidType** outBuffer, const std::string file, const std::string key)
{
	LPVoidType* buffer = _originVertexBufferDatas.Find(file, key);
	bool success = ( buffer != nullptr );

	if(outBuffer)
		(*outBuffer) = buffer;

	return success;
}

void BufferManager::DeleteVertexBuffer(const std::string& file, const std::string& key, bool remove)
{
	_vertexBuffers.Delete(file, key, remove);
}

void BufferManager::DeleteIndexBuffer(const std::string& file, const std::string& key, bool remove)
{
	_indexBuffers.Delete(file, key, remove);
}

void BufferManager::DeleteOriginVBData(const std::string& file, const std::string& key, bool remove)
{
	_originVertexBufferDatas.Delete(file, key, remove);
}

void BufferManager::DeleteVertexBuffer(const std::string& file, bool remove)
{
	_vertexBuffers.Delete(file, remove);
}

void BufferManager::DeleteIndexBuffer(const std::string& file, bool remove)
{
	_indexBuffers.Delete(file, remove);
}

void BufferManager::DeleteOriginVBData(const std::string& file, bool remove)
{
	_originVertexBufferDatas.Delete(file, remove);
}

void BufferManager::DeleteAllVertexBuffer(bool remove)
{
	_vertexBuffers.DeleteAll(remove);
}

void BufferManager::DeleteAllIndexBuffer(bool remove)
{
	_indexBuffers.DeleteAll(remove);
}

void BufferManager::DeleteAllOriginVBData(bool remove)
{
	_originVertexBufferDatas.DeleteAll(remove);
}
