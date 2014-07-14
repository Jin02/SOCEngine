#pragma once

namespace Utility
{
	template<typename T>
	inline void Swap(T &a, T &b)
	{
		T temp;

		temp = a;
		a    = b;
		b    = temp;
	}

	inline int nextPOT(int x)
	{
		x = x - 1;
		x = x | (x >> 1);
		x = x | (x >> 2);
		x = x | (x >> 4);
		x = x | (x >> 8);
		x = x | (x >> 16);

		return x + 1;
	}

}

