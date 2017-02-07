#include "EffectiveUpdater.h"

using namespace Utility;

EffectiveUpdater::EffectiveUpdater(const std::vector<std::function<uint()>>& getCounterFuncs)
	: _getCounterFuncs(getCounterFuncs), _counters()
{
	for(const auto& iter : getCounterFuncs)
		_prevUpdateCounters.push_back( iter() - 1 );			
}

EffectiveUpdater::EffectiveUpdater(const std::vector<const Counter*>& counters)
        : _getCounterFuncs(), _counters(counters)
{
	for(const auto& iter : counters)
		_prevUpdateCounters.push_back( iter->GetCounter() - 1 );			
}

EffectiveUpdater::EffectiveUpdater(const std::vector<const Counter*>& counters, const std::vector<std::function<uint()>>& getCounterFuncs)
        : _getCounterFuncs(getCounterFuncs), _counters(counters)
{
	for(const auto& iter : getCounterFuncs)
		_prevUpdateCounters.push_back( iter() - 1);			

	for(const auto& iter : counters)
		_prevUpdateCounters.push_back( iter->GetCounter() - 1);	
}

EffectiveUpdater::~EffectiveUpdater()
{
}
	
bool EffectiveUpdater::UpdateTarget(const std::function<void()>& updateCallback)
{
	bool isNeedToUpdate = false;
	{
		auto UpdatePrevCounter = [](uint curCounter, uint& prevCounter) -> bool
		{
			bool isChanged 	= curCounter != prevCounter;
			if(isChanged)	prevCounter = curCounter;
			
			return isChanged;
		};	
		
		uint index = 0;
		for(const auto& iter : _getCounterFuncs)
			isNeedToUpdate |= UpdatePrevCounter(iter(), _prevUpdateCounters[index++]);
		
		for(const auto& iter : _counters)
			isNeedToUpdate |= UpdatePrevCounter(iter->GetCounter(), _prevUpdateCounters[index++]);
	}
	
	if(isNeedToUpdate)
		updateCallback();
	
	return isNeedToUpdate;
}
