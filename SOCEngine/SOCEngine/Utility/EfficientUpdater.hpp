#pragma once

namespace Utility
{
	template<typename T>
	class EfficientUpdater
	{
	private:
		const T* 	_target;
		uint 		_prevUpdateCounter;
	
		const std::function<uint(const T* target)>& _getCounter;
		
	public:
		EfficientUpdater(const T* target, const std::function<uint(const T* target)>& getUpdateCounter)
			: _target(target), _prevUpdateCounter(getUpdateCounter(target) - 1), _getCounter(getUpdateCounter) { }
		~EfficientUpdater() {}
		
		bool UpdateTarget(const std::function<void(const T* target)>& updateCallback)
		{
			uint curCounter = _getCounter(_target);
	
			bool isChanged = curCounter != _prevUpdateCounter;
			if(isChanged)
			{
				updateCallback(_target);
				_prevUpdateCounter = curCounter;
			}
			
			return isChanged;					
		}
	};
}
