#pragma once

#include "SOCHashMap.h"

#include <string>
#include "SingleTon.h"

namespace Rendering
{
	class TagManager : public Utility::SingleTon<TagManager>
	{
	protected:
		SOCHashMap<std::string, int> hash;

	public:
		bool AddTag(std::string key, int id);
		bool FindTagValue(std::string key, int *outValue);
	};

}