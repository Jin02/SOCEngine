#include "OriginObjectManager.h"

using namespace Resource;

OriginObjectManager::OriginObjectManager()
{
}

OriginObjectManager::~OriginObjectManager()
{

}

void OriginObjectManager::Add(const std::string& key, const Core::Object* object)
{
	ASSERT_COND_MSG(Find(key) == nullptr, "Error, Duplicated key");
	_map.insert(std::make_pair(key, object));
}

const Core::Object* OriginObjectManager::Find(const std::string& key)
{
	auto findIter = _map.find(key);
	return findIter != _map.end() ? findIter->second : nullptr;
}

void OriginObjectManager::Delete(const std::string& key)
{
	auto findIter = _map.find(key);
	if(findIter != _map.end())
	{
		SAFE_DELETE(findIter->second);
		_map.erase(findIter);
	}
}

void OriginObjectManager::DeleteAll()
{
	for(auto iter = _map.begin(); iter != _map.end(); ++iter)
		SAFE_DELETE(iter->second);

	_map.clear();
}