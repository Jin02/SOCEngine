#include "ConstBufferManager.h"

using namespace Rendering::Buffer;

ConstBufferManager::ConstBufferManager()
{
}

ConstBufferManager::~ConstBufferManager()
{
}

ConstBuffer* ConstBufferManager::AddBuffer(unsigned int bufferSize)
{
	return AddBuffer(std::to_string(bufferSize) ,bufferSize);
}

ConstBuffer* ConstBufferManager::AddBuffer(const std::string& key, unsigned int bufferSize)
{
	ConstBuffer* cb = nullptr;
	{
		cb = _hash.Find(key);
		if(cb)	return cb;
	}

	cb = new ConstBuffer;
	if(cb->Create(bufferSize) == false)
	{
		SAFE_DELETE(cb);
		return nullptr;
	}
	_hash.Add(key,cb);

	return cb;
}

ConstBuffer* ConstBufferManager::Find(unsigned int bufferSize)
{
	return _hash.Find(std::to_string(bufferSize));
}

ConstBuffer* ConstBufferManager::Find(const std::string& key)
{
	return _hash.Find(key);
}

void ConstBufferManager::Remove(unsigned int bufferSize)
{
	_hash.Delete(std::to_string(bufferSize), true);
}

void ConstBufferManager::Remove(const std::string& key)
{
	_hash.Delete(key, true);
}

void ConstBufferManager::RemoveAll()
{
	_hash.DeleteAll(true);
}
