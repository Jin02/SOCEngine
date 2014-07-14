#pragma once

#include "TagManager.h"

namespace Rendering
{
	class LayerManager : protected TagManager
	{

	public:
		bool AddLayer(std::string key, int id);
		bool FindLayerValue(std::string key, int *outValue);
	};

}