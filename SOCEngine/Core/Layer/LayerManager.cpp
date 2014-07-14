#include "LayerManager.h"

using namespace std;

namespace Rendering
{
	bool LayerManager::AddLayer(std::string key, int id)
	{
		return TagManager::AddTag(key, id);
	}

	bool LayerManager::FindLayerValue(std::string key, int *outValue)
	{
		return TagManager::FindTagValue(key, outValue);
	}
}