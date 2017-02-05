#pragma once

namespace Utility
{
	class Counter
	{
	private:
		uint _counter;
		
	public:
		Counter(uint initCount) : _counter(initCount) {}
		~Counter() { }
		
	public:
		inline void AddCount() { ++_counter; }
		GET_ACCESSOR(Counter, uint, _counter);
	};
}
