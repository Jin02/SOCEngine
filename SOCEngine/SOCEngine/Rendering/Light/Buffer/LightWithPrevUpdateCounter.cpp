#include "LightWithPrevUpdateCounter.h"

using namespace Rendering;
using namespace Rendering::Light;

LightWithPrevUpdateCounter::LightWithPrevUpdateCounter(const LightForm* light, uint prevTransformUpdateCounter)
	: _light(light), _prevTransformUpdateCounter(prevTransformUpdateCounter)
{
}

LightWithPrevUpdateCounter::~LightWithPrevUpdateCounter()
{
}

bool LightWithPrevUpdateCounter::UpdateBuffer(const std::function<void(const LightForm* light)>& bufferUpdateCallback)
{
	uint curCounter = _light->GetOwner()->GetTransform()->GetUpdateCounter();

	bool isChanged = curCounter != _prevTransformUpdateCounter;
	if(isChanged)
	{
		bufferUpdateCallback(_light);
		_prevTransformUpdateCounter = curCounter;
	}
	
	return isChanged;
}
