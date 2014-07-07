#pragma once

namespace Utility
{
	template<typename T>
	inline void SAFE_DELETE(T& p){if(p){delete p; p=nullptr;}}

	template<typename T>
	inline void SAFE_ARRARY_DELETE(T& p){if(p){delete[] p; p=nullptr;}}

	template<typename T>
	inline void SAFE_RELEASE(T& p){if(p){p->Release(); p = nullptr;}}

	template<typename T>
	inline T Min(T a, T b){ return a < b ? a : b;}

	template<typename T>
	inline T Max(T a, T b){ return a > b ? a : b;}

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

