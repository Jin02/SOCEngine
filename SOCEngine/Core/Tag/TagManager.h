#pragma once

#include "Structure.h"

namespace Rendering
{
	class TagManager : public Singleton<TagManager>
	{
	protected:
		HashMap<std::string, int> hash;

	public:
		bool AddTag(std::string key, int id);
		bool FindTagValue(std::string key, int *outValue);
	};

}