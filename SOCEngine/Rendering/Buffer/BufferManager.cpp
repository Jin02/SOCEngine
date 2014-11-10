#include "BufferManager.h"

using namespace Rendering;

Buffer::Manager::Manager()
{
}

Buffer::Manager::~Manager()
{
	DeleteAllIndexBuffer(true);
	DeleteAllVertexBuffer(true);
	DeleteAllOriginVBData(true);
}

void Buffer::Manager::Add(const std::string file, const std::string key, Buffer::VertexBuffer* bufferData, bool copy)
{
	_vertexBuffers.Add(file, key, bufferData, copy);
}

void Buffer::Manager::Add(const std::string file, const std::string key, Buffer::IndexBuffer* bufferData, bool copy)
{
	_indexBuffers.Add(file, key, bufferData, copy);
}

void Buffer::Manager::Add(const std::string file, const std::string key, LPVoidType* bufferData, bool copy)
{
	_originVertexBufferDatas.Add(file, key, bufferData, copy);
}

bool Buffer::Manager::Find(Buffer::VertexBuffer*& outBuffer, const std::string file, const std::string key)
{
	outBuffer = _vertexBuffers.Find(file, key);
	return (outBuffer != nullptr);
}

bool Buffer::Manager::Find(Buffer::IndexBuffer*& outBuffer, const std::string file, const std::string key)
{
	outBuffer = _indexBuffers.Find(file, key);
	return (outBuffer != nullptr);
}

bool Buffer::Manager::Find(LPVoidType*& outBuffer, const std::string file, const std::string key)
{
	outBuffer = _originVertexBufferDatas.Find(file, key);
	return (outBuffer != nullptr);
}

void Buffer::Manager::DeleteVertexBuffer(const std::string& file, const std::string& key, bool remove)
{
	_vertexBuffers.Delete(file, key, remove);
}

void Buffer::Manager::DeleteIndexBuffer(const std::string& file, const std::string& key, bool remove)
{
	_indexBuffers.Delete(file, key, remove);
}

void Buffer::Manager::DeleteOriginVBData(const std::string& file, const std::string& key, bool remove)
{
	_originVertexBufferDatas.Delete(file, key, remove);
}

void Buffer::Manager::DeleteVertexBuffer(const std::string& file, bool remove)
{
	_vertexBuffers.Delete(file, remove);
}

void Buffer::Manager::DeleteIndexBuffer(const std::string& file, bool remove)
{
	_indexBuffers.Delete(file, remove);
}

void Buffer::Manager::DeleteOriginVBData(const std::string& file, bool remove)
{
	_originVertexBufferDatas.Delete(file, remove);
}

void Buffer::Manager::DeleteAllVertexBuffer(bool remove)
{
	_vertexBuffers.DeleteAll(remove);
}

void Buffer::Manager::DeleteAllIndexBuffer(bool remove)
{
	_indexBuffers.DeleteAll(remove);
}

void Buffer::Manager::DeleteAllOriginVBData(bool remove)
{
	_originVertexBufferDatas.DeleteAll(remove);
}
