#pragma once

#include "Utility.h"

namespace Utility
{
	template< class T >
	class SingleTon
	{
	private:
		static T* instance;

	public:
		static T* GetInstance()
		{
			if(instance == NULL)
				instance = new T();

			return instance;
		}

		static void DeleteSingleTon()
		{
			Utility::SAFE_DELETE(instance);
		}
	};

	template<class T>
	T* SingleTon<T>::instance = NULL;
}