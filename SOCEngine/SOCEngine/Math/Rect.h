#pragma once
#include "Size.h"

namespace Math
{
	template <typename Type>
	struct Rect
	{
	public:
		Type x, y;
		Size<Type> size;

	public:
		Rect()
		{
			x = 0;
			y = 0;
		}

		Rect(Type x, Type y, Type w, Type h)
		{
			this->x = x;
			this->y = y;
			this->size.w = w;
			this->size.h = h;
		}
	};

}