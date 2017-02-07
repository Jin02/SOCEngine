#pragma once

namespace Utility
{
	class EffectiveUpdater
	{
	public:
		EffectiveUpdater(const std::vector<std::function<uint()>>& getCounterFuncs);
		EffectiveUpdater(const std::vector<const Counter*>& counters);
		EffectiveUpdater(const std::vector<const Counter*>& counters, const std::vector<std::function<uint()>>& getCounterFuncs);
		~EffectiveUpdater();
		
	public:
		bool UpdateTarget(const std::function<void()>& updateCallback);

	private:
		const std::vector<std::function<uint()>>	_getCounterFuncs;
		const std::vector<const Counter*>		_counters;
		
		std::vector<uint>				_prevUpdateCounters;
	};
}
