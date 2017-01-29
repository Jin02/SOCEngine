#pragma once

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			class LightWithPrevUpdateCounter
			{
			private:
				const Light::LightForm*	_light;
				uint					_prevTransformUpdateCounter;
				
			public:
				LightWithPrevUpdateCounter(const Light::LightForm* light, uint prevTransformUpdateCounter);
				~LightWithPrevUpdateCounter();
				
				bool UpdateBuffer(const std::function<void(const Light::LightForm* light)>& bufferUpdateCallback);
			};		
		}
	}
}
