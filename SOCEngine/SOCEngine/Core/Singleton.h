#pragma once

#include "Common.h"

namespace Core
{
	template< class T >
	class Singleton
	{
	public:
		DISALLOW_ASSIGN(Singleton);

		static T* SharedInstance()
		{
			static T* instance = new T();
			return instance;
		}
	};
}
