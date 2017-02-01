#pragma once

namespace Rendering
{
	namespace Light
	{
		namespace LightingBuffer
		{
			template<typename LightType>
			class LightWithPrevUpdateCounter
			{
			private:
				const LightType*	_light;
				uint			_prevTransformUpdateCounter;
				
			public:
				LightWithPrevUpdateCounter(const LightType* light, uint prevTransformUpdateCounter)
					: _light(light), _prevTransformUpdateCounter(prevTransformUpdateCounter) { }
				~LightWithPrevUpdateCounter() {}
				
				bool UpdateBuffer(const std::function<void(const LightType* light)>& bufferUpdateCallback)
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
			};		
		}
	}
}
