#include "TagManager.h"

using namespace std;

namespace Rendering
{
	bool TagManager::AddTag(std::string key, int id)
	{
		SOCHashMap<string, int>::iterator iter = hash.find(key);

		if(iter != hash.end())
			return false;

		hash.insert(SOCHashMap<string, int>::value_type(key, id));

		return true;
	}
	bool TagManager::FindTagValue(std::string key, int *outValue)
	{
		SOCHashMap<string, int>::iterator iter = hash.find(key);

		if(iter == hash.end())
			return false;

		(*outValue) = iter->second;

		return true;
	}
}